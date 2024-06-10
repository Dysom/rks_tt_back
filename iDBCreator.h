#pragma once

#include <string>

class IDBCreator {
protected:
	std::string dbName;
public:
	virtual void createDataBaseIfNeeded() = 0;
	void setName(const std::string& dbName_) {
		dbName = dbName_;
	}
	virtual ~IDBCreator() {}
};
