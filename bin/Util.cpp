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

std::string get_first_file_name(const std::string& folder, const std::string & suffix)
{
	/*
	Find name of first file in folder
	*/
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	std::string file_name;

	hFind = FindFirstFile((folder + "/*" + suffix).c_str(), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		file_name = "";
	}
	else
	{
		file_name = FindFileData.cFileName;
		FindClose(hFind);
	}
	return file_name;
}


bool pointSortPredicate(const Shx& a, const Shx& b)
{
	if (a.r < b.r)
		return true;
	else if (a.r > b.r)
		return false;
	else if (a.c < b.c)
		return true;
	else
		return false;
};

bool pointComparisonPredicate(const Shx& a, const Shx& b)
{
	return a.r == b.r && a.c == b.c;
}