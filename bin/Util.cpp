#include "Util.h"
#include "windows.h"

std::vector<std::string> get_file_names(std::string folder, const std::string& suffix)
{
	std::vector<std::string> names;
	folder += "/*"+suffix;

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(folder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			if ((fd.dwFileAttributes) && fd.cFileName[0] != '.') {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
	return names;
}

std::vector<std::string> get_folder_names(std::string folder)
{
	std::vector<std::string> names;
	folder += "/*";

	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(folder.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fd.cFileName[0] != '.') {
				names.push_back(fd.cFileName);
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
	return names;
}
