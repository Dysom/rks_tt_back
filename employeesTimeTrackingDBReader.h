#pragma once

#include "constants.h"
#include "iData.h"
#include "specificData.h"

#include "sqlite3.h"

#include <memory>
#include <map>
#include <string>

class IDBReader {
protected:
	std::string dbName;
public:	
	void setName(const std::string& dbName_) {
		dbName = dbName_;
	}
	virtual ~IDBReader() {}
};

class EmployeesTimeTrackingDBReader : public IDBReader {	
	using DepartmentRow = EmployeesTimeTrackingRow::Department;
	using EmployeeRow = EmployeesTimeTrackingRow::Employee;
	using TimeTrackingRow = EmployeesTimeTrackingRow::TimeTracking;
public:
	template<typename T>
	std::shared_ptr<IData> ReadData(const std::string & query, int (*callback)(void* data, int argc, char** argv, char** azColName), void (*callAfterOpen) (sqlite3 * db_, int & rc_) = nullptr ) {

		sqlite3* db;
		char* errorMessage = 0;
		int rc;

		rc = sqlite3_open((dbName +".db").c_str(), &db);

		if (callAfterOpen != nullptr) {
			callAfterOpen(db, rc);
		}

		std::vector<T> data;

		if (rc == SQLITE_OK) {

			rc = sqlite3_exec(db, query.c_str(), callback, &data, &errorMessage);

			if (rc != SQLITE_OK) {
				sqlite3_free(errorMessage);
			}

			sqlite3_close(db);
		}

		return std::make_shared<SpecificData<T>>(std::move(data));
	}
	EmployeesTimeTrackingDBReader() {
		setName(DEFAULT_DB_NAME);
	}
	std::shared_ptr<IData> ReadAllDeparments();
	std::shared_ptr<IData> ReadAllEmployees();
	static std::map<std::string, unsigned long long> getMapFromData(IData* iDataPtr);
};
