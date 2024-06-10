#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <direct.h>
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <codecvt>
#include <sstream>
#include <time.h>

#include "constants.h"

#include "utils.h"



namespace utils {

	std::wstring utf8stringToWstring(const std::string& str)
	{
		//std::cout << "in stringToWstring\n";

		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		std::wstring outStr = converterX.from_bytes(str);

		//std::cout << "out stringToWstring\n";

		return outStr;
	}
	std::string wstringToUtf8String(const std::wstring& wstr)
	{
		using convert_typeX = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_typeX, wchar_t> converterX;
		return converterX.to_bytes(wstr);
	}

	std::string GetModuleDirectory() {
		char szPath[MAX_PATH] = {};
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		char* lstChr = strrchr(szPath, '\\');
		*lstChr = '\0';
		return szPath;
	}

	std::string GetModulePath() {
		char szPath[MAX_PATH] = {};
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		return szPath;
	}

	std::string GetCurrentWorkingDirectory() {
		const int BUFSIZE = 4096;
		char buf[BUFSIZE];
		memset(buf, 0, BUFSIZE);
		_getcwd(buf, BUFSIZE - 1);
		return buf;
	}

	bool ChangeCurrentWorkingDirectory(const std::string & newDir) {
		return !_chdir(newDir.c_str());
	}

	bool SetCurrentWorkingDirectoryToModuleDirectoryIfNeeded() {
		std::string moduleDirectory = utils::GetModuleDirectory();

		return utils::GetCurrentWorkingDirectory() == moduleDirectory || utils::ChangeCurrentWorkingDirectory(moduleDirectory);
	}

	bool createAndWaitProcess(const std::string& executeFilePath, const std::vector<std::string>& arguments) {

		std::wstring executeFilePathWStr = std::wstring(executeFilePath.begin(), executeFilePath.end());


		std::string argumentsStr = "";
		wchar_t* argsPtr = nullptr;

		if (arguments.size()) {
			argumentsStr = "\C";

			for (auto& arg : arguments) {
				argumentsStr += " ";
				argumentsStr += arg;
			}
		}

		std::wstring argumentsWStr = utf8stringToWstring(argumentsStr);
		
		std::vector<wchar_t> argumentsCharVector(argumentsWStr.c_str(), argumentsWStr.c_str() + argumentsWStr.size() + 1);

		if (arguments.size()) {
			argsPtr = &argumentsCharVector[0];
		}

		STARTUPINFO cif;
		ZeroMemory(&cif, sizeof(STARTUPINFO));
		PROCESS_INFORMATION pi;

		if (CreateProcessW(executeFilePathWStr.c_str(), argsPtr,
			NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi) == true) {
			WaitForSingleObject(pi.hProcess, INFINITE);

			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

			return true;
		}

		return false;
	}

	std::vector<std::string> getFileListOfDirectory(const std::string & directoryPath) {

		std::vector<std::string> filesNames;

		WIN32_FIND_DATA ffd;
		LARGE_INTEGER filesize;
		TCHAR szDir[MAX_PATH];
		size_t length_of_arg;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError = 0;		

		std::wstring dirPathW = std::wstring(directoryPath.begin(), directoryPath.end());

		StringCchCopy(szDir, MAX_PATH, dirPathW.c_str());
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

		// Find the first file in the directory.

		hFind = FindFirstFile(szDir, &ffd);

		if (INVALID_HANDLE_VALUE == hFind)
		{
			return {};
		}

		// List all the files in the directory with some info about them.

		do
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::wstring fileNameW = ffd.cFileName;

				std::string fileName = wstringToUtf8String(fileNameW);			

				filesNames.push_back(fileName);
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();

		if (dwError != ERROR_NO_MORE_FILES)
		{
			//DisplayErrorBox(TEXT("FindFirstFile"));
		}

		FindClose(hFind);		

		return filesNames;
	}

	std::vector<std::string> splitString(const std::string& s, char delim) {
		std::stringstream ss(s);
		std::string item;

		std::vector<std::string> elems;

		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}

		return elems;
	}

	unsigned long long parseDateTimeStr(const std::string& dateTime) {
		//31.07.2023 08:34:13
		//unsigned dayOfMonth = dateTime

		return 0;
	}
	
	unsigned long long highResolutionTimeNow()
	{
		struct timespec ts;

		if (timespec_get(&ts, TIME_UTC) != TIME_UTC)
		{
			fputs("timespec_get failed!", stderr);
			return 0;
		}

		return 1000000000 * ts.tv_sec + ts.tv_nsec;
	}

	DateTime getDateTimeFromString(const std::string & dateStr) {

		DateTime dT;

		int r = sscanf(dateStr.c_str(), "%d.%d.%d %d:%d", &dT.day, &dT.month, &dT.year, &dT.hours, &dT.minutes);

		return dT;
	}

	bool isLeapYear(unsigned year) {

		if (year % 4) {
			return false;
		}

		if (year % 100 == 0) {
			if (year % 400) {
				return false;
			}
		}

		return true;
	}

	unsigned long long getMillisecondsSince1970(const DateTime & dT) {
		int daysPerMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		if (isLeapYear(dT.year)) {
			daysPerMonth[2] = 29;
		}

		unsigned long long result = 0;

		for (unsigned year = 1970; year < dT.year; year++) {
			result += isLeapYear(year) ? 366 : 365;
		}

		for (unsigned month = 1; month < dT.month; month++) {
			result += daysPerMonth[month];
		}

		result += (dT.day - 1);

		result *= (3600 * 24);

		result += (3600 * dT.hours);

		result += (60 * dT.minutes);

		result += (dT.seconds);

		result *= 1000;

		result += dT.milliseconds;

		return result;
	}

	std::string getFileExtensionFromFilePath(const std::string& filePath) {

		std::string fileExtension = filePath.substr(filePath.find_last_of(".") + 1);
		
		if ((int)fileExtension.find('\\') < 0) {

			return fileExtension;
		}

		return "";
	}

	void removeFilesFromInputFilesFolder(const std::vector<std::string> & filesNames) {

		for (auto& fileName : filesNames) {

			std::string filePath = INPUT_FILES_FOLDER + "\\" + fileName;

			std::wstring filePathW = utf8stringToWstring(filePath);

			//remove((INPUT_FILES_FOLDER + "\\" + fileName).c_str());
			_wremove(filePathW.c_str());
		}
	}

	std::string readFileToString(const std::string& filePath) {
		std::ifstream in(filePath, std::ios::in | std::ios::binary);

		if (in) {
			std::ostringstream contents;
			contents << in.rdbuf();
			in.close();
			return contents.str();
		}

		return "";
	}

	std::string getMimeTypeOfFileExtension(const std::string& fileExtension) {
		static std::unordered_map<std::string, std::string> mimeTypes = {
			{"txt", "text/plain"},
			{"xml", "text/xml"},
			{"htm", "text/html"},
			{"html", "text/html"},
			{"css", "text/css"},
			{"js", "text/javascript"},
			{"png", "image/png"},
			{"svg", "image/svg+xml"},
			{"jpg", "image/jpeg"},
			{"jpeg", "image/jpeg"},
			{"gif", "image/gif"}
		};

		if (mimeTypes.count(fileExtension)) {
			return mimeTypes.at(fileExtension);
		}

		return "text/plain";
	}
}


