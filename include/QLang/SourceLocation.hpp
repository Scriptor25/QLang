#pragma once

#include <string>

namespace QLang
{
	struct SourceLocation
	{
		std::string Filename;
		size_t Row = 1, Column = 0;
	};
}
