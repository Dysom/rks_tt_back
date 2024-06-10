#pragma once

#include <string>
#include <vector>

namespace utils {

	struct DateTime {
		unsigned year = 1970;
		unsigned month = 1;
		unsigned day = 1;
		unsigned hours = 0;
		unsigned minutes = 0;
		unsigned seconds = 0;
		unsigned milliseconds = 0;

		static bool SameDates(const DateTime& dT, const DateTime& otherDT) {
			return (dT.year == otherDT.year) && (dT.month == otherDT.month) && (dT.day == otherDT.day);
		}

		unsigned getCountOfMinutesSinceStartDay() const {
			return (unsigned)hours * 60 + (unsigned)minutes;
		}
	};

	std::wstring utf8stringToWstring(const std::string& str);
	std::string wstringToUtf8String(const std::wstring& wstr);

	std::string GetModuleDirectory();

	std::string GetModulePath();

	std::string GetCurrentWorkingDirectory();

	bool ChangeCurrentWorkingDirectory(const std::string& newDir);

	bool SetCurrentWorkingDirectoryToModuleDirectoryIfNeeded();

	bool createAndWaitProcess(const std::string& executeFilePath, const std::vector<std::string>& arguments);

	std::vector<std::string> getFileListOfDirectory(const std::string& directoryPath);

	std::vector<std::string> splitString(const std::string& s, char delim);

	unsigned long long highResolutionTimeNow();

	DateTime getDateTimeFromString(const std::string& dateStr);

	unsigned long long getMillisecondsSince1970(const DateTime& dT);

	std::string getFileExtensionFromFilePath(const std::string& filePath);

	void removeFilesFromInputFilesFolder(const std::vector<std::string>& filesNames);

	std::string readFileToString(const std::string& filePath);

	std::string getMimeTypeOfFileExtension(const std::string& fileExtension);
}
