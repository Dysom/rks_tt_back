#include "timeTrackingCvsFileReader.h"
#include "specificData.h"
#include "utils.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>



std::shared_ptr<IData> TimeTrackingCVSFileReader::Read() {

	using Record = EmployeesTimeTrackingRow::ExcelTimeTrackingRecord;
		
	std::vector<Record> data;

	std::ifstream filein(utils::utf8stringToWstring(filePath));

	bool isFirst = true;

	for (std::string line; std::getline(filein, line);)	{		

		if (isFirst == false) {
			auto fields = utils::splitString(line, ',');

			Record record;

			record.dateTime = utils::getDateTimeFromString(fields[1]);
			record.dateTimeStamp = utils::getMillisecondsSince1970(record.dateTime);
			record.department = fields[3];
			record.position = fields[4];
			record.surname = fields[5];
			record.name = fields[6];
			record.patronymic = fields[7];
			record.type = fields[9];

			data.push_back(record);
		}

		isFirst = false;
	}

	std::sort(begin(data), end(data), [](Record & rec, Record & otherRec) {
		return rec.dateTimeStamp < otherRec.dateTimeStamp;
	});

	return std::make_shared<SpecificData<EmployeesTimeTrackingRow::ExcelTimeTrackingRecord>>(std::move(data));
}
