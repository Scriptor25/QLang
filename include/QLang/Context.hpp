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

		void AddIncludeDir(const std::filesystem::path &);
		bool FindInIncludeDirs(
			std::filesystem::path &dest, const std::string &filename);
		bool AddParsed(const std::filesystem::path &);

	private:
		std::map<std::string, TypePtr> m_Types;

		std::vector<std::filesystem::path> m_IncludeDirs;
		std::vector<std::filesystem::path> m_Parsed;
	};
}
