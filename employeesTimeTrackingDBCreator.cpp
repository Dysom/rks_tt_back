
#include "employeesTimeTrackingDBCreator.h";

#include "sqlite3.h"
#include <iostream>
#include <string>

void EmployeesTimeTrackingDBCreator::executeQuery(const std::string& query) {
    // Pointer to SQLite connection
    sqlite3* db;

    // Save any error messages
    char* zErrMsg = 0;

    // Save the result of opening the file
    int rc;

    // Save the result of opening the file
    rc = sqlite3_open((dbName + ".db").c_str(), &db);

    if (rc) {
        // Show an error message
        std::cout << "DB Error: " << sqlite3_errmsg(db) << "\n";
        // Close the connection
        sqlite3_close(db);
        // Return an error
        return;
    }

    // Run the SQL (convert the string to a C-String with c_str() )
    rc = sqlite3_exec(db, query.c_str(), 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {

        std::cout << "SQL error: " << zErrMsg << "\n";
        sqlite3_free(zErrMsg);
        sqlite3_close(db);
        return;
    }

    // Close the SQL connection
    sqlite3_close(db);
}

void EmployeesTimeTrackingDBCreator::createDataBaseIfNeeded() {
    //// Pointer to SQLite connection
    //sqlite3* db;

    //// Save any error messages
    //char* zErrMsg = 0;

    //// Save the result of opening the file
    //int rc;


    //// Save the result of opening the file
    //rc = sqlite3_open((dbName + ".db").c_str(), &db);

    //if (rc) {
    //    // Show an error message
    //    std::cout << "DB Error: " << sqlite3_errmsg(db) << "\n";
    //    // Close the connection
    //    sqlite3_close(db);
    //    // Return an error
    //    return;
    //}

    //// Save any SQL
    //std::string sql;

    //// Save SQL to create a table
    //    
    //sql = "CREATE TABLE IF NOT EXISTS departments (department_id INTEGER PRIMARY KEY, name TEXT NOT NULL);";
    //sql += "CREATE TABLE IF NOT EXISTS employees (employee_id INTEGER PRIMARY KEY, surname TEXT NOT NULL, name TEXT, patronymic TEXT, position TEXT, department_id INTEGER);";
    //sql += "CREATE TABLE IF NOT EXISTS timeTracking (date INTEGER NOT NULL, employee_id INTEGER NOT NULL, job_time INTEGER NOT NULL, PRIMARY KEY (date, employee_id));";

    //// Run the SQL (convert the string to a C-String with c_str() )
    //rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);
    //    
    //if (rc != SQLITE_OK) {
    //        
    //    std::cout << "SQL error: " << zErrMsg << "\n";
    //    sqlite3_free(zErrMsg);
    //    sqlite3_close(db);
    //    return;
    //}

    //// Close the SQL connection
    //sqlite3_close(db);

    std::string query = "CREATE TABLE IF NOT EXISTS departments (department_id INTEGER PRIMARY KEY, name TEXT NOT NULL);";
    query += "CREATE TABLE IF NOT EXISTS employees (employee_id INTEGER PRIMARY KEY, surname TEXT NOT NULL, name TEXT, patronymic TEXT, position TEXT, department_id INTEGER);";
    query += "CREATE TABLE IF NOT EXISTS timeTracking (date INTEGER NOT NULL, employee_id INTEGER NOT NULL, job_time INTEGER NOT NULL, PRIMARY KEY (date, employee_id));";

    executeQuery(query);
}

void EmployeesTimeTrackingDBCreator::clearAllTables() {

    std::string query = "DELETE FROM timeTracking; ";
    query += "DELETE FROM employees; ";
    query += "DELETE FROM departments;";

    executeQuery(query);
}
