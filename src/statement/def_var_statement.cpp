#include <iostream>
#include <ostream>
#include <utility>
#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Operator.hpp>
#include <QLang/QLang.hpp>
#include <QLang/Statement.hpp>
#include <QLang/Type.hpp>
#include <QLang/Value.hpp>

QLang::DefVarStatement::DefVarStatement(const SourceLocation& where,
                                        const bool is_extern,
                                        const TypePtr& type,
                                        const std::string& name,
                                        StatementPtr init)
    : DefVarStatement(where, is_extern, type, name, dyn_cast<Expression>(std::move(init)))
{
}

QLang::DefVarStatement::DefVarStatement(const SourceLocation& where,
                                        const bool is_extern,
                                        const TypePtr& type,
                                        const std::string& name,
                                        std::vector<StatementPtr> args)
    : DefVarStatement(where, is_extern, type, name, dyn_cast<Expression>(args))
{
}

QLang::DefVarStatement::DefVarStatement(const SourceLocation& where,
                                        const bool is_extern,
                                        TypePtr type,
                                        std::string name,
                                        ExpressionPtr init)
    : Statement(where),
      IsExtern(is_extern),
      Type(std::move(type)),
      Name(std::move(name)),
      Init(std::move(init))
{
}

QLang::DefVarStatement::DefVarStatement(const SourceLocation& where,
                                        const bool is_extern,
                                        TypePtr type,
                                        std::string name,
                                        std::vector<ExpressionPtr> args)
    : Statement(where),
      IsExtern(is_extern),
      Type(std::move(type)),
      Name(std::move(name)),
      Args(std::move(args))
{
}

std::ostream& QLang::DefVarStatement::Print(std::ostream& stream) const
{
    stream << "def ";
    if (IsExtern) stream << "ext ";
    stream << Type << ' ' << Name;

    if (Init) return stream << " = " << Init;

    if (Args.empty()) return stream;

    stream << " { ";
    for (size_t i = 0; i < Args.size(); ++i)
    {
        if (i > 0) stream << ", ";
        stream << Args[i];
    }
    return stream << " }";
}

void QLang::DefVarStatement::GenIRVoid(Builder& builder) const
{
    builder.SetLoc(Where);

    if (!builder.IRBuilder().GetInsertBlock())
    {
        if (!Init)
        {
            const auto ir_type = Type->GenIR(builder);
            llvm::Constant* init = nullptr;
            if (!IsExtern) init = llvm::Constant::getNullValue(ir_type);

            const auto ptr = new llvm::GlobalVariable(builder.IRModule(),
                                                      ir_type,
                                                      false,
                                                      IsExtern
                                                          ? llvm::GlobalValue::ExternalLinkage
                                                          : llvm::GlobalValue::InternalLinkage,
                                                      init,
                                                      Name);
            builder[Name] = LValue::Create(builder, Type, ptr);
            return;
        }

        std::cerr << "at " << Where << ": global variables are not supported" << std::endl;
        return;
    }

    ValuePtr init;
    if (Init)
    {
        init = Init->GenIR(builder);
        if (!init)
        {
            std::cerr << "    at " << Where << std::endl;
            return;
        }
    }

    LValuePtr instance;
    if (const auto ref_type = ReferenceType::From(Type))
    {
        const auto l_init = LValue::From(init);
        if (!l_init)
        {
            std::cerr << "at " << Where << ": initializer must be a lvalue here" << std::endl;
            return;
        }
        instance = LValue::Create(builder, ref_type->GetBase(), l_init->GetPtr());
    }
    else
    {
        instance = builder.CreateInstance(Type, Name);
        if (!instance)
        {
            std::cerr << "    at " << Where << std::endl;
            return;
        }

        if (init)
        {
            init = GenCast(builder, init, Type);
            if (!init)
            {
                std::cerr << "    at " << Where << std::endl;
                return;
            }

            instance->Set(init->Get());
        }
        else
        {
            std::vector<ValuePtr> args;
            std::vector<TypePtr> arg_types;
            for (const auto& arg : Args)
            {
                auto ir_arg = arg->GenIR(builder);
                args.push_back(ir_arg);
                arg_types.push_back(ir_arg->GetType());
            }

            if (const auto func = builder.FindConstructor(Type, arg_types))
            {
                init = GenCall(builder, func->AsValue(builder), instance, args);
                if (!init)
                {
                    std::cerr << "    at " << Where << std::endl;
                    return;
                }
            }
        }
    }

    builder[Name] = instance;
}
