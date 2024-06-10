#include "employeesTimeTrackingDBWriter.h"

#include "constants.h"
#include "specificData.h"
#include "utils.h"

#include "sqlite3.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>



class DBTransactExecutor {
	sqlite3* db;
	char* errMsg;
	int recordsLimit;
	int recordsCnt = 0;
	int rc;
	std::string transactionStr;
	void prepareTransaction() {
		recordsCnt = 0;
		transactionStr = "BEGIN TRANSACTION; ";
	}
	void finalizeAndExecuteTransaction() {
		transactionStr += "COMMIT; ";

		rc = sqlite3_exec(db, transactionStr.c_str(), 0, 0, &errMsg);

		prepareTransaction();
	}
public:
	DBTransactExecutor(sqlite3 * db_, char * errMsg_, int rc_, int recordsLimit_ = 50) : db(db_), errMsg(errMsg_), rc(rc_), recordsLimit(recordsLimit_) {		
		prepareTransaction();
	}
	void writeRecord(const std::string& query) {
		transactionStr += query;
		recordsCnt++;

		if (recordsCnt == recordsLimit) {
			finalizeAndExecuteTransaction();
		}
	}
	void flush() {
		if (recordsCnt >  0) {
			finalizeAndExecuteTransaction();
		}
	}
	bool isGood() const {
		return rc == SQLITE_OK;
	}
};

bool EmployeesTimeTrackingDBWriter::writeAndClearDataVectors() {

	sqlite3* db;
	char* errMsg = 0;
		
	int rc = sqlite3_open((dbName + ".db").c_str(), &db);
	
	DBTransactExecutor transactExecutor(db, errMsg, rc);

	if (transactExecutor.isGood()) {

		for (auto& row : departmentsData) {

			transactExecutor.writeRecord("INSERT OR REPLACE INTO departments(department_id, name) VALUES(" + std::to_string(row.department_id) + ", '" + row.name + "'); ");
		}

		transactExecutor.flush();
	}

	if (transactExecutor.isGood()) {

		for (auto& row : employeesData) {

			transactExecutor.writeRecord("INSERT OR REPLACE INTO employees(employee_id, department_id, surname, name, patronymic, position) VALUES("
				+ std::to_string(row.employee_id)
				+ ", "
				+ std::to_string(row.department_id)
				+ ", '" + row.surname + "'"
				+ ", '" + row.name + "'"
				+ ", '" + row.patronymic + "'"
				+ ", '" + row.position + "'); ");
		}

		transactExecutor.flush();
	}

	if (transactExecutor.isGood()) {

		for (auto& row : timeTrackingData) {

			transactExecutor.writeRecord("INSERT OR REPLACE INTO timeTracking(date, employee_id, job_time) VALUES("
				+ std::to_string(row.dateTimeStamp)
				+ ", " + std::to_string(row.employee_id)
				+ ", " + std::to_string(row.job_time) + "); ");
		}

		transactExecutor.flush();
	}

	if (transactExecutor.isGood() == false) {
		sqlite3_free(errMsg);
	}	
		
	sqlite3_close(db);

	departmentsData.clear();
	employeesData.clear();
	timeTrackingData.clear();

	return transactExecutor.isGood();
}

void EmployeesTimeTrackingDBWriter::write(IData* iDataPtr) {

	if (dynamic_cast<SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>*>(iDataPtr)) {

		auto mapDepartments = EmployeesTimeTrackingDBReader::getMapFromData(reader.ReadAllDeparments().get());
		auto mapEmployees = EmployeesTimeTrackingDBReader::getMapFromData(reader.ReadAllEmployees().get());
				
		unsigned long long nextDepartmentId = utils::highResolutionTimeNow();
		unsigned long long nextEmploymentId = utils::highResolutionTimeNow();

		std::unordered_map<unsigned long long, unsigned> inputTime, outputTime; // key - uid of emp, value - count of minutes since the beginning of the day

		unsigned long long uidOfCurrentEmployee;

		utils::DateTime currentDateTime;

		SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>* dataPtr = dynamic_cast<SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>*>(iDataPtr);

		auto& data = dataPtr->getData();

		for (auto& row : data) {

			std::string fio = row.surname + " " + row.name + " " + row.patronymic;

			if (mapDepartments.count(row.department) == 0) {

				EmployeesTimeTrackingRow::Department record;

				record.department_id = nextDepartmentId++;
				record.name = row.department;

				mapDepartments[record.name] = record.department_id;

				departmentsData.push_back(record);
			}

			if (mapEmployees.count(fio) == 0) {

				EmployeesTimeTrackingRow::Employee record;

				record.employee_id = nextEmploymentId++;				

				uidOfCurrentEmployee = record.employee_id;

				record.department_id = mapDepartments.at(row.department);
				record.surname = row.surname;
				record.name = row.name;
				record.patronymic = row.patronymic;
				record.position = row.position;

				mapEmployees[fio] = record.employee_id;

				employeesData.push_back(record);
			}
			else {
				uidOfCurrentEmployee = mapEmployees.at(fio);
			}

			if (utils::DateTime::SameDates(currentDateTime, row.dateTime) == false) {

				auto currentDateTimeStamp = utils::getMillisecondsSince1970(currentDateTime);

				for (auto& kV : inputTime) {
					if (outputTime.count(kV.first)) {
						EmployeesTimeTrackingRow::TimeTracking record;

						record.dateTimeStamp = currentDateTimeStamp;
						record.employee_id = kV.first;
						record.job_time = outputTime.at(kV.first) - kV.second;

						if (record.job_time > 0) {

							timeTrackingData.push_back(record);
							
							if (timeTrackingData.size() > 100) {

								writeAndClearDataVectors();
							}
						}
					}
				}

				currentDateTime = row.dateTime;
			}

			unsigned countOfMinutesSinceStartDay = row.dateTime.getCountOfMinutesSinceStartDay();

			//auto wType = utils::stringToWstring(row.type);

			//bool isOutput = (wType[1] == L'û' || wType[1] == L'Û');
			bool isOutput = (row.type == "âûõîä" || row.type == "ÂÛÕÎÄ" || row.type == u8"âûõîä" || row.type == u8"ÂÛÕÎÄ");

			if (isOutput == false) {

				if (inputTime.count(uidOfCurrentEmployee)) {
					inputTime[uidOfCurrentEmployee] = std::min(countOfMinutesSinceStartDay, inputTime.at(uidOfCurrentEmployee));
				}
				else {
					inputTime[uidOfCurrentEmployee] = countOfMinutesSinceStartDay;
				}
			}
			else {
				if (outputTime.count(uidOfCurrentEmployee)) {
					outputTime[uidOfCurrentEmployee] = std::max(countOfMinutesSinceStartDay, outputTime.at(uidOfCurrentEmployee));
				}
				else {
					outputTime[uidOfCurrentEmployee] = countOfMinutesSinceStartDay;
				}
			}
		}

		writeAndClearDataVectors();
	}
}

bool EmployeesTimeTrackingDBWriter::writeExcelFileFromInputFolder(const std::string& fileName) {

	excelFileReader.setFilePath(INPUT_FILES_FOLDER + "\\" + fileName);	

	auto sPtr = excelFileReader.Read();

	if (sPtr.get()->getStatus() != DataStatus::bad) {

		write(sPtr.get());

		return true;
	}

	return false;
}