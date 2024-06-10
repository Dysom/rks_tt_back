// rks_test_task.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>

#include "httplib.h"
#include "json.hpp"
#include "sqlite3.h"

#include "constants.h"
#include "employeesTimeTrackingDBCreator.h";
#include "employeesTimeTrackingDBWriter.h"
#include "employeesTimeTrackingDBReader.h"
#include "process.h"
#include "specificData.h"
#include "sqlite_extensions.h"
#include "utils.h"




int main(int argc, char** argv) {

	SetConsoleOutputCP(CP_UTF8);	
	std::setlocale(LC_ALL, "C.UTF-8");		
	
	utils::SetCurrentWorkingDirectoryToModuleDirectoryIfNeeded();
	
	EmployeesTimeTrackingDBCreator databaseCreator;

	databaseCreator.createDataBaseIfNeeded();
		
	EmployeesTimeTrackingDBWriter databaseWriter;

	EmployeesTimeTrackingDBReader databaseReader;


	{
		using json = nlohmann::json;

		httplib::Server server;

		//std::mutex commandsMutex;
		
		server.Get("/", [](const httplib::Request& req, httplib::Response& res) {
			auto binaryStringOfFile = utils::readFileToString(REACT_DIST_FOLDER + "\\index.html");

			if (binaryStringOfFile.size()) {
				res.set_content(binaryStringOfFile, "text/html");
			}
			else {
				res.set_content("Файл 'index.html' не существует или его не удалось открыть", "text/plain");
			}
		});

		server.Get(R"(/.+\.(\w+))", [&](const httplib::Request& req, httplib::Response& res) {			

			auto strPath = req.matches[0].str();
			auto fileExtension = req.matches[1].str();

			std::replace(strPath.begin(), strPath.end(), '/', '\\');

			auto filePath = REACT_DIST_FOLDER + strPath;

			//std::cout << "filePath: {" << filePath << "} {" << utils::getMimeTypeOfFileExtension(fileExtension) << "}\n";

			auto binaryStringOfFile = utils::readFileToString(filePath);

			if (binaryStringOfFile.size()) {
				res.set_content(binaryStringOfFile, utils::getMimeTypeOfFileExtension(fileExtension));
			}			
			else {
				res.set_content("{" + strPath + "} не существует или его не удалось открыть", "text/plain");
			}
		});

		server.Get("/test", [](const httplib::Request& req, httplib::Response& res) {						
			res.set_content("test", "text/plain");
		});

		server.Post("/commands", [&databaseCreator, &databaseWriter, &databaseReader](const httplib::Request& req, httplib::Response& res) {

			//std::lock_guard<std::mutex> lockCommands(commandsMutex);

			json requestJson = json::parse(req.body);

			std::string commandName = requestJson["command"];
			
			json responseJson;

			//std::cout << "commandName: " << commandName << "\n";

			if (commandName == "get_files_names_of_input_files_directory") {

				auto filesNames = utils::getFileListOfDirectory(INPUT_FILES_FOLDER);

				for (auto& fileName : filesNames) {
					responseJson.push_back(fileName);					
				}
			}
			else if (commandName == "remove_files_from_input_files_directory") {

				auto filesNames = requestJson["names"];

				std::vector<std::string> removedFiles;

				for (auto& fileName : filesNames) {					
					removedFiles.push_back(fileName);
				}

				utils::removeFilesFromInputFilesFolder(removedFiles);

				responseJson.push_back("OK");
			}
			else if (commandName == "clear_all_tables_in_datebase") {

				databaseCreator.clearAllTables();

				responseJson.push_back("OK");
			}
			else if (commandName == "write_excel_files_to_database_from_input_files_directory") {

				auto filesNames = requestJson["names"];

				responseJson["writed_files"] = {};

				for (auto& fileName : filesNames) {
					if (databaseWriter.writeExcelFileFromInputFolder(fileName)) {
						responseJson["writed_files"].push_back(fileName);
					}
				}

				//responseJson.push_back("OK");
			}
			else if (commandName == "select_all_departmens_employees_and_minmax_dates") {

				{
					using MinmaxPair = std::pair<unsigned long long, unsigned long long>;

					auto dataSPtr = databaseReader.ReadData<MinmaxPair>("SELECT MIN(date), MAX(date) FROM timeTracking", [](void* data, int argc, char** argv, char** azColName) -> int {

						auto dataPtr = static_cast<std::vector<MinmaxPair>*>(data);

						MinmaxPair record;

						record.first = argv[0] ? std::stoull(argv[0]) : 0;
						record.second = argv[1] ? std::stoull(argv[1]) : 0;

						if (record.first > record.second) {
							std::swap(record.first, record.second);
						}

						dataPtr->push_back(record);

						return 0;
						});

					auto dataPtr = dynamic_cast<SpecificData<MinmaxPair> *>(dataSPtr.get());

					unsigned long long minDateTimestamp = 0;
					unsigned long long maxDateTimestamp = 0;

					if (dataPtr->getData().size()) {
						minDateTimestamp = dataPtr->getData()[0].first;
						maxDateTimestamp = dataPtr->getData()[0].second;
					}

					responseJson["minDateTimestamp"] = minDateTimestamp;
					responseJson["maxDateTimestamp"] = maxDateTimestamp;
				}

				{
					using DepartmentRow = EmployeesTimeTrackingRow::Department;

					auto dataSPtr = databaseReader.ReadAllDeparments();

					auto dataPtr = dynamic_cast<SpecificData<DepartmentRow> *>(dataSPtr.get());

					responseJson["departments"] = {};

					for (auto& record : dataPtr->getData()) {
						responseJson["departments"].push_back({ {"department_id", std::to_string(record.department_id)}, {"name", record.name} });
					}
				}

				{
					using EmployeeRow = EmployeesTimeTrackingRow::Employee;

					auto dataSPtr = databaseReader.ReadAllEmployees();

					auto dataPtr = dynamic_cast<SpecificData<EmployeeRow> *>(dataSPtr.get());

					responseJson["employees"] = {};

					for (auto& record : dataPtr->getData()) {						

						responseJson["employees"].push_back({
							{"employee_id", std::to_string(record.employee_id)},
							{"department_id", std::to_string(record.department_id)},
							{"surname", record.surname},
							{"name", record.name},
							{"patronymic", record.patronymic},
							{"position", record.position}
						});

						//std::cout << "surname: {" << record.surname << "}\n";
					}					
				}

				//std::cout << "<<" << responseJson.dump() << ">>\n";

				//std::cout << "test-str\n";

				responseJson["OK"];
			}
			else if (commandName == "select_timeTracking") {

				std::string startDateStr = requestJson["startDate"];
				std::string endDateStr = requestJson["endDate"];

				auto startDate = std::stoull(startDateStr);
				auto endDate = std::stoull(endDateStr);
				auto employeesIds = requestJson["employeesIds"];

				std::string aggregateFunc = "AVG";				

				void (*callAfterOpen) (sqlite3* db_, int& rc_) = nullptr;

				if (requestJson.count("aggregateFunc")) {
					aggregateFunc = requestJson["aggregateFunc"];
				}

				if (aggregateFunc == "MEDIAN") {

					callAfterOpen = sqlite_extensions::appendFuncOfMedianToDb;
				}

				responseJson["used_aggregate_func"] = aggregateFunc;

				using EmpIdWorkTimePair = std::pair<unsigned long long, unsigned long long>;

				std::string whereCondition = "(date >= " + std::to_string(startDate) + " AND date < " + std::to_string(endDate + 86400000) + ")";

				if (employeesIds.size()) {

					whereCondition += " AND (employee_id IN (";

					bool nF = false;

					for (auto& employeeId : employeesIds) {
						if (nF) {
							whereCondition += ", ";
						}

						whereCondition += employeeId;

						nF = true;
					}

					whereCondition += "))";
				}


				std::string query = "SELECT employee_id, " + aggregateFunc + "(job_time) FROM timeTracking WHERE " + whereCondition + " GROUP BY employee_id";

				//std::cout << "query: {" + query + "}\n";

				{
					auto dataSPtr = databaseReader.ReadData<EmpIdWorkTimePair>(query, [](void* data, int argc, char** argv, char** azColName) -> int {

						auto dataPtr = static_cast<std::vector<EmpIdWorkTimePair>*>(data);

						EmpIdWorkTimePair record;

						record.first = std::stoull(argv[0]);
						record.second = std::stoull(argv[1]);

						dataPtr->push_back(record);

						return 0;
					}, callAfterOpen);

					auto dataPtr = dynamic_cast<SpecificData<EmpIdWorkTimePair> *>(dataSPtr.get());

					responseJson["time_tracks"] = {};

					auto& data = dataPtr->getData();

					if (data.size()) {						

						for (auto& rec : data) {

							responseJson["time_tracks"].push_back({ {"employee_id", std::to_string(rec.first)}, {"time", std::to_string(rec.second)}});
						}

					}
				}
			}
			else {
				responseJson.push_back("unkown command");
				responseJson.push_back(commandName);
			}

			std::string responseDumpString = responseJson.dump();

			//std::cout << "<" << responseDumpString.length() << ">\n";

			res.set_header("Access-Control-Allow-Origin", "*");


			res.set_content(responseDumpString, "application/json");
		});

		server.Post("/upload_file", [](const httplib::Request& req, httplib::Response& res) {

			auto loaded_file = req.get_file_value("uploaded_file");

			{
				std::string filePath = INPUT_FILES_FOLDER + "\\" + loaded_file.filename;
				std::wstring filePathW = utils::utf8stringToWstring(filePath);
								
				std::ofstream ofs(filePathW.c_str(), std::ios::binary);
				ofs << loaded_file.content;
			}

			json responseJson;

			responseJson["load_status"] = "OK";

			res.set_header("Access-Control-Allow-Origin", "*");
			res.set_content(responseJson.dump(), "application/json");
		});

		/*server.Get("(/stop)", [&server](const httplib::Request& req, httplib::Response& res) {

			res.set_content(u8"Сервер остановлен", "text/plain");
			server.stop();
		});*/

		std::thread thread_1([&server]() {

			server.listen("0.0.0.0", 8080);

			std::cout << u8"Сервер остановлен\n";
		});

		std::chrono::milliseconds timespan(100);

		while (server.is_running() == false) {
			if (server.is_valid() == false) {
				break;
			}

			std::this_thread::sleep_for(timespan);
		}

		if (server.is_running()) {

			std::cout << u8"Сервер запущен\n";
			std::cout << u8"Для остановки сервера введите любой непробельный символ и нажмите ввод\n";

			char ch_;

			std::cin >> ch_;

			server.stop();

			thread_1.join();
		}
		else {
			std::cout << u8"Ошибка при запуске сервера\n";
		}
	}


	return 0;
}