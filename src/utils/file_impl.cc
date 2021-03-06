#include "file.h"

#include "api_mask.h"
#include "utils/include_fs_library.h"

#ifndef HAS_FILESYSTEM_LIBRARY
#  include "utils/file_win.inc"
#  include "utils/file_unix.inc"
#else //HAS_FILESYSTEM_LIBRARY

namespace stego_disk {

std::vector<stego_disk::File> File::GetFilesInDir(std::string_view directory, std::string_view filter /*= ""*/)
{
	std::vector<File> ret;
	for (auto &i: fs::recursive_directory_iterator(directory)) {
		const fs::path & i_path = i.path();
		//PSTODO skusit neplatne symlinky
		if (fs::status(i).type() != fs::file_type::regular) //PSTODO chceme aj nejake ine?
			continue;

		auto parent_path = i_path.parent_path().string();
		auto filename = i_path.filename().string();

		if (std::regex rx(".*\\.(" + std::string(filter) + ")$", std::regex_constants::icase); filter.empty())
		{
#ifndef HAS_FILESYSTEM_LIBRARY
			ret.emplace_back(File(parent_path, filename));
#else
			ret.emplace_back(File(i_path));
#endif
		}
		else
		{
			if (std::regex_match(filename, rx))
			{
#ifndef HAS_FILESYSTEM_LIBRARY
				ret.emplace_back(File(parent_path, filename));
#else
				ret.emplace_back(File(i_path));
#endif
			}
		}
	}
	return ret;
}

} // stego_disk

#endif //HAS_FILESYSTEM_LIBRARY
