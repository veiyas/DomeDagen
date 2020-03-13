#include <string>
#include <filesystem>

class Utility
{
public:
	static std::string findRootDir()
	{
		auto searcher = std::filesystem::current_path();
		while (searcher.has_parent_path())
		{
			if (searcher.filename() == "DomeDagen")
				return searcher.string();
			else
				searcher = searcher.parent_path();
		}
		return "";
	}

};