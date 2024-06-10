#include "timeTrackingExcelFileReader.h"

#include <iostream>
#include <memory>

#include "specificData.h"
#include "utils.h"

std::shared_ptr<IData> TimeTrackingExcelFileReader::Read() {

	std::string fileExtension = utils::getFileExtensionFromFilePath(filePath);

	if (fileExtension == "csv") {

		cVSFileReader.setFilePath(filePath);

		return cVSFileReader.Read();
	}
	else if (fileExtension == "xlsx" || fileExtension == "xls") {

		xLSFileReader.setFilePath(filePath);

		return xLSFileReader.Read();
	}

	auto outputDataSPtr = std::make_shared<SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>>();

	outputDataSPtr.get()->setStatus(DataStatus::bad);

	return outputDataSPtr;
}