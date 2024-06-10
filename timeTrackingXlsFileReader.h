#pragma once

#include "iReader.h"
#include "timeTrackingCvsFileReader.h"

#include <memory>

class TimeTrackingXLSFileReader : public IFileReader {
	TimeTrackingCVSFileReader cVSFileReader;
public:	
	std::shared_ptr<IData> Read() override;
};
