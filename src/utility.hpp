#pragma once

#include <string>
#include <filesystem>
#include <iostream>

class Utility
{
public:
	static std::string findRootDir()
	{
		auto searcher = std::filesystem::current_path();
		while (searcher.has_parent_path())
		{
			if (searcher.filename() == "DomeDagen" || searcher.filename() == "Domedagen" || searcher.filename() == "domedagen")
				return searcher.string();
			else
				searcher = searcher.parent_path();
		}
		return "";
	}

};