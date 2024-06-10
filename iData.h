#pragma once

#include <string>

enum DataStatus {good, bad, unknown};

class IData {
	DataStatus status = good;
public:
	//virtual std::string getDataTypeName() const = 0;
	virtual ~IData() {}
	virtual void setStatus(DataStatus status_) {
		status = status_;
	}
	virtual DataStatus getStatus() const {
		return status;
	}
};
