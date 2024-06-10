#include <algorithm>
#include <iostream>
#include <vector>

#include "sqlite3.h"


#include "sqlite_extensions.h"

namespace sqlite_extensions {
	struct Spec_Data {
		std::vector<unsigned>* vecPtr = nullptr;
	};

	unsigned calculateMedian(std::vector<unsigned>& values) {
		size_t size = values.size();
		if (size == 0) {
			return 0;
		}

		std::sort(values.begin(), values.end());

		if (size % 2 == 0) {
			auto sumOf2 = values[size / 2 - 1] + values[size / 2];
			return sumOf2 / 2 + (sumOf2 % 2 ? 1 : 0);
		}
		else {
			return values[size / 2];
		}
	}


	void medianStep(sqlite3_context* context, int argc, sqlite3_value** argv) {

		if (sqlite3_value_type(argv[0]) == SQLITE_NULL) {
			return;
		}

		Spec_Data* ptr;

		ptr = static_cast<Spec_Data*>(sqlite3_aggregate_context(context, sizeof(*ptr)));

		if (ptr == nullptr) {
			sqlite3_result_error(context, "Ошибка выделения памяти", -1);
			return;
		}

		if (ptr->vecPtr == nullptr) {
			ptr->vecPtr = new std::vector<unsigned>();
		}

		unsigned long long value = sqlite3_value_int64(argv[0]);

		ptr->vecPtr->push_back(value);
	}

	void medianFinal(sqlite3_context* context) {

		Spec_Data* ptr;

		ptr = static_cast<Spec_Data*>(sqlite3_aggregate_context(context, 0));

		if (ptr != nullptr) {

			unsigned median = calculateMedian(*(ptr->vecPtr));

			sqlite3_result_int64(context, median);

			delete ptr->vecPtr;
		}
	}

	void appendFuncOfMedianToDb(sqlite3* db, int& rc) {

		rc = sqlite3_create_function(db, "MEDIAN", 1, SQLITE_UTF8, nullptr, nullptr, medianStep, medianFinal);

		if (rc != SQLITE_OK) {
			std::cout << "Ошибка при создании функции: " << sqlite3_errmsg(db) << "\n";
			sqlite3_close(db);
		}
	}
}