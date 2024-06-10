
#include "employeesTimeTrackingDBReader.h"

#include "iData.h"
#include "specificData.h"

#include "sqlite3.h"

#include <iostream>
#include <memory>
#include <string>
#include <map>
#include <vector>



std::shared_ptr<IData> EmployeesTimeTrackingDBReader::ReadAllDeparments() {

	return ReadData<DepartmentRow>("SELECT department_id, name FROM departments", [](void* data, int argc, char** argv, char** azColName) -> int {

		std::vector<DepartmentRow>* dataPtr = static_cast<std::vector<DepartmentRow>*>(data);

		DepartmentRow record;

		for (int i = 0; i < argc; i++) {
			if (std::string(azColName[i]) == "department_id")
				record.department_id = std::stoull(argv[i]);
			else if (std::string(azColName[i]) == "name")
				record.name = argv[i];		
		}

		dataPtr->push_back(record);

		return 0;
		});
}

std::shared_ptr<IData> EmployeesTimeTrackingDBReader::ReadAllEmployees() {

	return ReadData<EmployeeRow>("SELECT employee_id, department_id, surname, name, patronymic, position FROM employees", [](void* data, int argc, char** argv, char** azColName) -> int {

		std::vector<EmployeeRow>* dataPtr = static_cast<std::vector<EmployeeRow>*>(data);

		EmployeeRow record;

		for (int i = 0; i < argc; i++) {
			if (std::string(azColName[i]) == "employee_id")
				record.employee_id = std::stoull(argv[i]);
			else if (std::string(azColName[i]) == "department_id")
				record.department_id = std::stoull(argv[i]);
			else if (std::string(azColName[i]) == "surname")
				record.surname = argv[i];
			else if (std::string(azColName[i]) == "name")
				record.name = argv[i];
			else if (std::string(azColName[i]) == "patronymic")
				record.patronymic = argv[i];
			else if (std::string(azColName[i]) == "position")
				record.position = argv[i];
		}

		dataPtr->push_back(record);

		return 0;
	});
}

std::map<std::string, unsigned long long> EmployeesTimeTrackingDBReader::getMapFromData(IData* iDataPtr) {

	std::map<std::string, unsigned long long> map;

	if (dynamic_cast<SpecificData<EmployeesTimeTrackingRow::Department> *>(iDataPtr)) {

		auto dataPtr = dynamic_cast<SpecificData<EmployeesTimeTrackingRow::Department> *>(iDataPtr);

		auto& data = dataPtr->getData();

		for (auto& row : data) {
			map[row.name] = row.department_id;
		}
	}
	else if (dynamic_cast<SpecificData<EmployeesTimeTrackingRow::Employee> *>(iDataPtr)) {

		auto dataPtr = dynamic_cast<SpecificData<EmployeesTimeTrackingRow::Employee> *>(iDataPtr);

		auto& data = dataPtr->getData();

		for (auto& row : data) {
			map[row.surname + " " + row.name + " " + row.patronymic] = row.employee_id;
		}
	}

	return map;
}