#pragma once

#include <QLang/QLang.hpp>
#include <map>
#include <string>

namespace QLang
{
	class Context
	{
	public:
		Context();

		TypePtr &GetType(const std::string &);

	private:
		std::map<std::string, TypePtr> m_Types;
	};
}
