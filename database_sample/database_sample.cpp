/*****************************************************************************\
BSD 3-Clause License

Copyright (c) 2024, kcenon
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\*****************************************************************************/

/**
 * @file database_sample.cpp
 * @brief Demonstrates basic database operations with SQLite
 *
 * This sample shows how to:
 * - Connect to SQLite database
 * - Create tables (DDL)
 * - Insert records (CREATE)
 * - Query records (READ)
 * - Update records (UPDATE)
 * - Delete records (DELETE)
 * - Handle database results
 */

#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <vector>
#include <map>
#include <functional>

#include <sqlite3.h>

// Simple database result types
using database_row = std::map<std::string, std::string>;
using database_result = std::vector<database_row>;

// Simple SQLite wrapper class
class simple_sqlite_db {
public:
    simple_sqlite_db() : db_(nullptr) {}

    ~simple_sqlite_db() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    bool connect(const std::string& db_path) {
        int rc = sqlite3_open(db_path.c_str(), &db_);
        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite Error] " << sqlite3_errmsg(db_) << "\n";
            return false;
        }
        return true;
    }

    bool execute(const std::string& sql) {
        char* err_msg = nullptr;
        int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg);

        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite Error] " << err_msg << "\n";
            sqlite3_free(err_msg);
            return false;
        }
        return true;
    }

    database_result query(const std::string& sql) {
        database_result results;

        auto callback = [](void* data, int argc, char** argv, char** col_names) -> int {
            auto* results_ptr = static_cast<database_result*>(data);
            database_row row;

            for (int i = 0; i < argc; ++i) {
                row[col_names[i]] = argv[i] ? argv[i] : "NULL";
            }

            results_ptr->push_back(row);
            return 0;
        };

        char* err_msg = nullptr;
        int rc = sqlite3_exec(db_, sql.c_str(), callback, &results, &err_msg);

        if (rc != SQLITE_OK) {
            std::cerr << "[SQLite Error] " << err_msg << "\n";
            sqlite3_free(err_msg);
        }

        return results;
    }

    int get_last_insert_id() {
        return static_cast<int>(sqlite3_last_insert_rowid(db_));
    }

    int get_changes() {
        return sqlite3_changes(db_);
    }

private:
    sqlite3* db_;
};

// Helper function to display database results
void display_results(const database_result& results, const std::string& title) {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│  " << std::left << std::setw(54) << title << "│\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n\n";

    if (results.empty()) {
        std::cout << "   (No records found)\n\n";
        return;
    }

    // Display each row
    int row_num = 1;
    for (const auto& row : results) {
        std::cout << "Record #" << row_num++ << ":\n";

        for (const auto& [column, value] : row) {
            std::cout << "   " << std::setw(15) << std::left << column << ": " << value << "\n";
        }
        std::cout << "\n";
    }
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "  Database Sample (SQLite)\n";
    std::cout << "=================================================\n\n";

    // Create database connection
    std::cout << "[Database] Creating SQLite database...\n";
    simple_sqlite_db db;

    // Connect to database (in-memory for this sample)
    std::cout << "[Database] Connecting to in-memory database...\n";
    if (!db.connect(":memory:")) {
        std::cerr << "[Error] Failed to connect to database!\n";
        return 1;
    }
    std::cout << "[Database] Connected successfully.\n\n";

    // ========================================
    // Phase 1: CREATE TABLE
    // ========================================
    std::cout << "[Phase 1] Creating 'users' table...\n";
    std::string create_table_sql = R"(
        CREATE TABLE users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT NOT NULL UNIQUE,
            email TEXT NOT NULL,
            age INTEGER,
            active INTEGER DEFAULT 1
        )
    )";

    if (!db.execute(create_table_sql)) {
        std::cerr << "[Error] Failed to create table!\n";
        return 1;
    }
    std::cout << "[Phase 1] Table 'users' created successfully.\n\n";

    // ========================================
    // Phase 2: INSERT (CREATE)
    // ========================================
    std::cout << "[Phase 2] Inserting sample users...\n";

    std::vector<std::string> insert_queries = {
        "INSERT INTO users (username, email, age, active) VALUES ('alice', 'alice@example.com', 28, 1)",
        "INSERT INTO users (username, email, age, active) VALUES ('bob', 'bob@example.com', 35, 1)",
        "INSERT INTO users (username, email, age, active) VALUES ('charlie', 'charlie@example.com', 42, 0)",
        "INSERT INTO users (username, email, age, active) VALUES ('diana', 'diana@example.com', 31, 1)",
        "INSERT INTO users (username, email, age, active) VALUES ('eve', 'eve@example.com', 29, 1)"
    };

    int inserted_count = 0;
    for (const auto& query : insert_queries) {
        if (db.execute(query)) {
            inserted_count++;
        }
    }
    std::cout << "[Phase 2] Inserted " << inserted_count << " users successfully.\n\n";

    // ========================================
    // Phase 3: SELECT (READ)
    // ========================================
    std::cout << "[Phase 3] Querying all users...\n";
    auto all_users = db.query("SELECT * FROM users ORDER BY id");
    display_results(all_users, "All Users");

    std::cout << "[Phase 3] Querying active users only...\n";
    auto active_users = db.query("SELECT * FROM users WHERE active = 1 ORDER BY age");
    display_results(active_users, "Active Users (Sorted by Age)");

    std::cout << "[Phase 3] Querying users with age >= 30...\n";
    auto senior_users = db.query("SELECT username, email, age FROM users WHERE age >= 30 ORDER BY age DESC");
    display_results(senior_users, "Users Age >= 30");

    // ========================================
    // Phase 4: UPDATE
    // ========================================
    std::cout << "[Phase 4] Updating user 'bob' age to 36...\n";
    if (db.execute("UPDATE users SET age = 36 WHERE username = 'bob'")) {
        int updated = db.get_changes();
        std::cout << "[Phase 4] Updated " << updated << " record(s).\n\n";
    }

    std::cout << "[Phase 4] Verifying update...\n";
    auto bob_record = db.query("SELECT * FROM users WHERE username = 'bob'");
    display_results(bob_record, "Bob's Updated Record");

    std::cout << "[Phase 4] Deactivating user 'eve'...\n";
    if (db.execute("UPDATE users SET active = 0 WHERE username = 'eve'")) {
        int updated = db.get_changes();
        std::cout << "[Phase 4] Updated " << updated << " record(s).\n\n";
    }

    std::cout << "[Phase 4] Verifying active users after deactivation...\n";
    active_users = db.query("SELECT username, active FROM users ORDER BY id");
    display_results(active_users, "All Users (Username & Active Status)");

    // ========================================
    // Phase 5: DELETE
    // ========================================
    std::cout << "[Phase 5] Deleting user 'charlie'...\n";
    if (db.execute("DELETE FROM users WHERE username = 'charlie'")) {
        int deleted = db.get_changes();
        std::cout << "[Phase 5] Deleted " << deleted << " record(s).\n\n";
    }

    std::cout << "[Phase 5] Verifying deletion...\n";
    all_users = db.query("SELECT * FROM users ORDER BY id");
    display_results(all_users, "Remaining Users After Deletion");

    // ========================================
    // Phase 6: Aggregate Queries
    // ========================================
    std::cout << "[Phase 6] Running aggregate queries...\n";

    auto count_result = db.query("SELECT COUNT(*) as total_users FROM users");
    if (!count_result.empty() && count_result[0].count("total_users")) {
        std::cout << "[Aggregate] Total users: " << count_result[0]["total_users"] << "\n";
    }

    auto avg_age = db.query("SELECT AVG(age) as avg_age FROM users");
    if (!avg_age.empty() && avg_age[0].count("avg_age")) {
        std::cout << "[Aggregate] Average age: " << avg_age[0]["avg_age"] << "\n";
    }

    auto active_count = db.query("SELECT COUNT(*) as active_count FROM users WHERE active = 1");
    if (!active_count.empty() && active_count[0].count("active_count")) {
        std::cout << "[Aggregate] Active users: " << active_count[0]["active_count"] << "\n";
    }
    std::cout << "\n";

    // Summary
    std::cout << "=================================================\n";
    std::cout << "  Database Operations Summary\n";
    std::cout << "=================================================\n";
    std::cout << "✓ Table Creation:     SUCCESS\n";
    std::cout << "✓ Record Insertion:   " << inserted_count << " records\n";
    std::cout << "✓ Record Queries:     Multiple SELECT operations\n";
    std::cout << "✓ Record Updates:     2 UPDATE operations\n";
    std::cout << "✓ Record Deletion:    1 DELETE operation\n";
    std::cout << "✓ Aggregate Queries:  COUNT, AVG\n";
    std::cout << "=================================================\n\n";

    std::cout << "[Database] Sample completed successfully.\n";

    return 0;
}
