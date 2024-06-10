#pragma once

#include "iData.h"
#include "utils.h"

#include <iostream>
#include <string>
#include <vector>

template <typename T>
class SpecificData : public IData {
	std::vector<T> data;
public:
	SpecificData(std::vector<T> && inData) : data(inData) {}
	SpecificData() {};
	/*std::string getDataTypeName() const {
		return "unknown";
	}*/
	std::vector<T>& getData() {
		return data;
	}
};

namespace EmployeesTimeTrackingRow {

	struct Department {
		unsigned long long department_id;
		std::string name;
	};

	struct Employee {
		unsigned long long employee_id;
		std::string surname;
		std::string name;
		std::string patronymic;
		std::string position;
		unsigned long long department_id;
	};

	struct TimeTracking {
		unsigned long long dateTimeStamp;
		unsigned long long employee_id;
		unsigned job_time;
	};

	struct ExcelTimeTrackingRecord {
		unsigned long long dateTimeStamp;
		utils::DateTime dateTime;
		std::string surname;
		std::string name;
		std::string patronymic;
		std::string position;
		std::string department;
		std::string type;
	};
}

//template <>
//std::string SpecificData<EmployeesTimeTrackingRow::Department>::getDataTypeName() const {
//	return "EmployeesTimeTrackingRow_Departments";
//}
//
//template <>
//std::string SpecificData<EmployeesTimeTrackingRow::Employee>::getDataTypeName() const {
//	return "EmployeesTimeTrackingRow_Employees";
//}
//
//template <>
//std::string SpecificData<EmployeesTimeTrackingRow::TimeTracking>::getDataTypeName() const {
//	return "EmployeesTimeTrackingRow_TimeTrackings";
//}
//
//template <>
//std::string SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>::getDataTypeName() const {
//	return "EmployeesTimeTrackingRow_ExcelTimeTrackingRecord";
//}
