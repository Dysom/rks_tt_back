#pragma once

#include "constants.h"
#include "iData.h"
#include "employeesTimeTrackingDBReader.h"
#include "timeTrackingExcelFileReader.h"
#include "specificData.h"

#include <string>

class IDBWriter {
protected:
	std::string dbName;
public:
	virtual void write(IData * data) = 0;
	void setName(const std::string& dbName_) {
		dbName = dbName_;
	}
	virtual ~IDBWriter() {};
};

class EmployeesTimeTrackingDBWriter : public IDBWriter {

	using DepartmentRow = EmployeesTimeTrackingRow::Department;
	using EmployeeRow = EmployeesTimeTrackingRow::Employee;
	using TimeTrackingRow = EmployeesTimeTrackingRow::TimeTracking;

	EmployeesTimeTrackingDBReader reader;
	TimeTrackingExcelFileReader excelFileReader;

	std::vector<DepartmentRow> departmentsData;
	std::vector<EmployeeRow> employeesData;
	std::vector<TimeTrackingRow> timeTrackingData;

	bool writeAndClearDataVectors();
public:
	EmployeesTimeTrackingDBWriter() {

		setName(DEFAULT_DB_NAME);
		reader.setName(DEFAULT_DB_NAME);
	}
	void write(IData* iDataPtr) override;
	bool writeExcelFileFromInputFolder(const std::string& fileName);
};