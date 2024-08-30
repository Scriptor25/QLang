#pragma once

#include <QLang/QLang.hpp>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace QLang
{
	class Context
	{
	public:
		Context();

		TypePtr &GetType(const std::string &);

		bool AddParsed(const std::filesystem::path &);

	private:
		std::map<std::string, TypePtr> m_Types;
		std::vector<std::filesystem::path> m_Parsed;
	};
}
