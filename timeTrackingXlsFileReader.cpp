#include "timeTrackingXlsFileReader.h"

#include <iostream>
#include <memory>

#include <stdio.h>

#include "constants.h"
#include "utils.h"



std::shared_ptr<IData> TimeTrackingXLSFileReader::Read() {

	std::string cvsFilePath = TEMP_FOLDER + "\\" + "temp_" + std::to_string(utils::highResolutionTimeNow()) + ".cvs";
	
	//utils::createAndWaitProcess("...\\xlsToCvsConsoleConverter\\bin\\Release\\net6.0\\xlsToCvsConsoleConverter.exe", { filePath, cvsFilePath });
	utils::createAndWaitProcess("xlsToCsvConverter\\xlsToCvsConsoleConverter.exe", { filePath, cvsFilePath });

	cVSFileReader.setFilePath(cvsFilePath);

	auto sPtrData = cVSFileReader.Read();

	remove(cvsFilePath.c_str());

	return sPtrData;
}