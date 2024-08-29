#include <QLang/Builder.hpp>
#include <QLang/Expression.hpp>
#include <QLang/Type.hpp>
#include <iostream>

QLang::NameExpression::NameExpression(
	const SourceLocation &where, const std::string &name)
	: Expression(where), Name(name)
{
}

std::ostream &QLang::NameExpression::Print(std::ostream &stream) const
{
	return stream << Name;
}

QLang::ValuePtr QLang::NameExpression::GenIR(Builder &builder) const
{
	if (builder.IsCallee())
	{
		builder.IsCallee() = false;

		if (Name == "self")
		{
			auto self = builder["self"]->GetType();
			auto ctor = builder.FindConstructor(self, builder.GetArgs());
			if (ctor)
				return RValue::Create(
					builder, PointerType::Get(ctor->Type), ctor->IR);

			std::cerr << "no constructor for " << self << " with args"
					  << std::endl;
			for (const auto &arg : builder.GetArgs())
				std::cerr << "    " << arg << std::endl;

			return {};
		}

		auto sym = builder[Name];
		if (sym)
		{
			if (sym->GetType()->IsPointer())
			{
				auto type = FunctionType::FromPtr(sym->GetType());
				if (type)
				{
					if (type->GetParamCount() == builder.GetArgCount()
						|| (type->IsVarArg()
							&& type->GetParamCount() < builder.GetArgCount()))
					{
						size_t i = 0;
						for (; i < type->GetParamCount(); ++i)
							if (type->GetParam(i) != builder.GetArg(i)) break;
						if (i == type->GetParamCount()) return sym;
					}
				}
			}
		}

		if (auto func = builder.FindFunction(Name, {}, builder.GetArgs()))
			return RValue::Create(
				builder, PointerType::Get(func->Type), func->IR);

		if (auto func = builder.FindConstructor(
				Type::Get(builder.GetContext(), Name), builder.GetArgs()))
			return RValue::Create(
				builder, PointerType::Get(func->Type), func->IR);

		std::cerr << "no function with name '" << Name << "' and args"
				  << std::endl;
		for (const auto &arg : builder.GetArgs())
			std::cerr << "    " << arg << std::endl;

		return {};
	}

	return builder[Name];
}
