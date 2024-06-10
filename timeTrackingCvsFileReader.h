#pragma once

#include "iReader.h"

class TimeTrackingCVSFileReader : public IFileReader {
	
public:
	std::shared_ptr<IData> Read() override;
};

