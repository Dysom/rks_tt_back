#pragma once

#include "iReader.h"
#include "timeTrackingCvsFileReader.h"
#include "timeTrackingXlsFileReader.h"

#include <memory>

class TimeTrackingExcelFileReader : public IFileReader {
	TimeTrackingCVSFileReader cVSFileReader;
	TimeTrackingXLSFileReader xLSFileReader;
public:
	std::shared_ptr<IData> Read() override;
};
