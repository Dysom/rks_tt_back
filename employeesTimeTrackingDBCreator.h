#pragma once

#include "constants.h"
#include "iDBCreator.h"

class EmployeesTimeTrackingDBCreator : public IDBCreator {
    void executeQuery(const std::string & query);
public:
    EmployeesTimeTrackingDBCreator() {
        setName(DEFAULT_DB_NAME);
    }
	void createDataBaseIfNeeded() override;
    void clearAllTables();
};