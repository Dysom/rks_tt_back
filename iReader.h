#pragma once

#include "iData.h"

#include <memory>
#include <string>

class IReader {
public:
	virtual std::shared_ptr<IData> Read() = 0;
	virtual ~IReader() {}
};

class IFileReader : IReader {
protected:
	std::string filePath;
public:
	virtual void setFilePath(const std::string& filePath_) {
		filePath = filePath_;
	}
};
