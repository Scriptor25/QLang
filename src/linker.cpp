#include <memory>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Linker/Linker.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <QLang/Builder.hpp>
#include <QLang/Linker.hpp>

QLang::Linker::Linker()
{
    m_IRContext = std::make_unique<llvm::LLVMContext>();
}

llvm::LLVMContext& QLang::Linker::IRContext() const { return *m_IRContext; }

void QLang::Linker::Print() const
{
    m_IRModule->print(llvm::errs(), nullptr);
}

void QLang::Linker::Link(Builder& builder)
{
    builder.Finalize();

    auto& module = builder.IRModulePtr();
    if (verifyModule(*module, &llvm::errs())) return;

    if (!m_IRModule)
    {
        m_IRModule = std::move(module);
        return;
    }

    m_IRModule->setModuleIdentifier(m_IRModule->getModuleIdentifier() + "," + module->getModuleIdentifier());
    m_IRModule->setSourceFileName(m_IRModule->getSourceFileName() + "," + module->getSourceFileName());
    llvm::Linker::linkModules(*m_IRModule, std::move(module));
}

void QLang::Linker::EmitObject(const std::string& filename) const
{
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    const auto triple = llvm::sys::getDefaultTargetTriple();

    std::string err;
    const auto target = llvm::TargetRegistry::lookupTarget(triple, err);

    if (!target)
    {
        llvm::errs() << err;
        return;
    }

    const auto cpu = "generic";
    const auto features = "";

    const llvm::TargetOptions opt;
    const auto machine = target->createTargetMachine(triple, cpu, features, opt, llvm::Reloc::PIC_);

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
