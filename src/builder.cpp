#include <QLang/Builder.hpp>
#include <QLang/Type.hpp>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/PassInstrumentation.h>
#include <llvm/IR/Value.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/TargetParser/TargetParser.h>
#include <llvm/Transforms/AggressiveInstCombine/AggressiveInstCombine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Utils/Mem2Reg.h>
#include <memory>
#include <system_error>

QLang::Builder::Builder(Context &context) : m_Context(context)
{
	m_IRContext = std::make_unique<llvm::LLVMContext>();
	m_IRBuilder = std::make_unique<llvm::IRBuilder<>>(*m_IRContext);
	m_IRModule = std::make_unique<llvm::Module>("module", *m_IRContext);

	m_FPM = std::make_unique<llvm::FunctionPassManager>();
	m_LAM = std::make_unique<llvm::LoopAnalysisManager>();
	m_FAM = std::make_unique<llvm::FunctionAnalysisManager>();
	m_CGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
	m_MAM = std::make_unique<llvm::ModuleAnalysisManager>();
	m_PIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
	m_SI = std::make_unique<llvm::StandardInstrumentations>(*m_IRContext, true);

	m_SI->registerCallbacks(*m_PIC, m_MAM.get());

	// m_FPM->addPass(llvm::InstCombinePass());
	m_FPM->addPass(llvm::AggressiveInstCombinePass());
	m_FPM->addPass(llvm::ReassociatePass());
	m_FPM->addPass(llvm::GVNPass());
	m_FPM->addPass(llvm::SimplifyCFGPass());
	m_FPM->addPass(llvm::PromotePass());

	llvm::PassBuilder pb;
	pb.registerModuleAnalyses(*m_MAM);
	pb.registerFunctionAnalyses(*m_FAM);
	pb.crossRegisterProxies(*m_LAM, *m_FAM, *m_CGAM, *m_MAM);
}

llvm::LLVMContext &QLang::Builder::IRContext() const { return *m_IRContext; }

llvm::IRBuilder<> &QLang::Builder::IRBuilder() const { return *m_IRBuilder; }

llvm::Module &QLang::Builder::IRModule() const { return *m_IRModule; }

QLang::Context &QLang::Builder::GetContext() const { return m_Context; }

void QLang::Builder::Optimize(llvm::Function *fn) { m_FPM->run(*fn, *m_FAM); }

void QLang::Builder::Dump() { m_IRModule->print(llvm::errs(), nullptr); }

void QLang::Builder::EmitObject(const std::string &filename)
{
	llvm::InitializeAllTargetInfos();
	llvm::InitializeAllTargets();
	llvm::InitializeAllTargetMCs();
	llvm::InitializeAllAsmParsers();
	llvm::InitializeAllAsmPrinters();

	auto triple = llvm::sys::getDefaultTargetTriple();

	std::string err;
	auto target = llvm::TargetRegistry::lookupTarget(triple, err);

	if (!target)
	{
		llvm::errs() << err;
		return;
	}

	auto cpu = "generic";
	auto features = "";

	llvm::TargetOptions opt;
	auto machine = target->createTargetMachine(
		triple, cpu, features, opt, llvm::Reloc::PIC_);

	m_IRModule->setDataLayout(machine->createDataLayout());
	m_IRModule->setTargetTriple(triple);

	std::error_code ec;
	llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);

	if (ec)
	{
		llvm::errs() << "could not open file: " << ec.message();
		return;
	}

	llvm::legacy::PassManager pass;
	if (machine->addPassesToEmitFile(
			pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile))
	{
		llvm::errs() << "machine cannot emit filetype";
		return;
	}

	pass.run(*m_IRModule);
	dest.flush();
}

void QLang::Builder::Push() { m_Stack.push_back(m_Values); }

void QLang::Builder::Pop()
{
	m_Values = m_Stack.back();
	m_Stack.pop_back();
}

QLang::ValuePtr &QLang::Builder::operator[](const std::string &name)
{
	return m_Values[name];
}

std::vector<QLang::ValuePtr> &QLang::Builder::DestroyAtEnd()
{
	return m_DestroyAtEnd;
}

bool &QLang::Builder::IsCallee() { return m_IsCallee; }

QLang::ValuePtr &QLang::Builder::Self() { return m_Self; }

void QLang::Builder::SetArgCount(size_t c) { m_Args.resize(c); }

size_t QLang::Builder::GetArgCount() { return m_Args.size(); }

QLang::TypePtr &QLang::Builder::GetArg(size_t i) { return m_Args[i]; }

const std::vector<QLang::TypePtr> &QLang::Builder::GetArgs() { return m_Args; }

QLang::TypePtr &QLang::Builder::GetResult() { return m_Result; }

QLang::Function &QLang::Builder::GetFunction(
	const std::string &name, const FunctionTypePtr &type)
{
	return m_Functions[name][type];
}

QLang::Function *QLang::Builder::FindFunction(
	const std::string &name, const TypePtr &self,
	const std::vector<TypePtr> &args)
{
	size_t low_error = -1;
	Function *result = nullptr;

	for (auto &[fn_type, fn] : m_Functions[name])
	{
		if (fn_type->GetSelf() != self) continue;
		if ((!fn_type->IsVarArg() && fn_type->GetParamCount() != args.size())
			|| fn_type->GetParamCount() > args.size())
			continue;

		size_t error = 0;
		for (size_t i = 0; i < fn_type->GetParamCount(); ++i)
			error += Type::TypeDiff(*this, fn_type->GetParam(i), args[i]);

		if (error < low_error)
		{
			low_error = error;
			result = &fn;
		}
	}

	return result;
}

QLang::Function *QLang::Builder::FindConstructor(
	const TypePtr &self, const std::vector<TypePtr> &args)
{
	size_t low_error = -1;
	Function *result = nullptr;

	for (auto &[fn_type, fn] : m_Functions[self->GetName()])
	{
		if (fn_type->GetMode() != FnMode_Ctor) continue;
		if ((!fn_type->IsVarArg() && fn_type->GetParamCount() != args.size())
			|| fn_type->GetParamCount() > args.size())
			continue;

		size_t error = 0;
		for (size_t i = 0; i < fn_type->GetParamCount(); ++i)
			error += Type::TypeDiff(*this, fn_type->GetParam(i), args[i]);

		if (error < low_error)
		{
			low_error = error;
			result = &fn;
		}
	}

	return result;
}

QLang::Function *QLang::Builder::FindDestructor(const TypePtr &self)
{
	auto &func = m_Functions[self->GetName()][FunctionType::Get(
		FnMode_Dtor, GetVoidTy(), self, {}, false)];

	if (!func.IR) return nullptr;
	return &func;
}

QLang::TypePtr QLang::Builder::GetVoidTy() const
{
	return Type::Get(m_Context, "void");
}

QLang::TypePtr QLang::Builder::GetInt1Ty() const
{
	return Type::Get(m_Context, "i1");
}

QLang::TypePtr QLang::Builder::GetInt8Ty() const
{
	return Type::Get(m_Context, "i8");
}

QLang::TypePtr QLang::Builder::GetInt16Ty() const
{
	return Type::Get(m_Context, "i16");
}

QLang::TypePtr QLang::Builder::GetInt32Ty() const
{
	return Type::Get(m_Context, "i32");
}

QLang::TypePtr QLang::Builder::GetInt64Ty() const
{
	return Type::Get(m_Context, "i64");
}

QLang::TypePtr QLang::Builder::GetFloat16Ty() const
{
	return Type::Get(m_Context, "f16");
}

QLang::TypePtr QLang::Builder::GetFloat32Ty() const
{
	return Type::Get(m_Context, "f32");
}

QLang::TypePtr QLang::Builder::GetFloat64Ty() const
{
	return Type::Get(m_Context, "f64");
}

QLang::PointerTypePtr QLang::Builder::GetVoidPtrTy() const
{
	return PointerType::Get(GetVoidTy());
}

QLang::PointerTypePtr QLang::Builder::GetInt8PtrTy() const
{
	return PointerType::Get(GetInt8Ty());
}
