#include <cstring>
#include <iostream>
#include <stdexcept>
#include <vector>
#include "todo.h"
using namespace std;

TODOApp::TODOApp() {
  int rc = sqlite3_open(db_filename, &db);

  if (rc)
    throw std::runtime_error("Unable to open database");

  const char *create_table = "create table if not exists task("
                             "id int primary key,"
                             "completed int,"
                             "name text(50));";

  sqlite3_exec(db, create_table, 0, 0, 0);

  sqlite3_stmt *res;
  sqlite3_prepare_v2(db, "select max(id) from task;", -1, &res, 0);
  rc = sqlite3_step(res);
  availableKey = 1 + sqlite3_column_int(res, 0);

  sqlite3_finalize(res);

  // cout << availableKey << endl;
}

TODOApp::~TODOApp() { sqlite3_close(db); }

void TODOApp::run() {
  running = true;
  cout << "Welcome to TODO App\n";
  cout << "1. Add Task\n2. Mark Task As Completed\n3. Print All Tasks\n4. "
          "Delete Task\n5. Exit\n"
       << endl;

  while (running) {
    int option;
    cout << "Enter an option: " << flush;
    ws(cin);
    cin >> option;

    switch (option) {
     case 1:
      addTask();
      break;

    case 2:
      markTaskAsCompleted();
      break;

    case 3:
      printTasks();
      break;

    case 4:
      removeTask();
      break;

    case 5:
      running = false;
      break;

    default:
      cout << "Wrong Option! Try again" << endl;
      break;
    }
  }
}

void TODOApp::addTask() {
  string buf;
  cout << "Enter Task: " << flush;
  ws(cin);
  getline(cin, buf);

  sqlite3_stmt *res;
  sqlite3_prepare_v2(
      db, "INSERT INTO task(id, completed, name) values(?, 0, ?)", -1, &res, 0);
  sqlite3_bind_int(res, 1, availableKey);
  sqlite3_bind_text(res, 2, buf.c_str(), -1, SQLITE_STATIC);
  sqlite3_step(res);
  sqlite3_finalize(res);
  availableKey++;

  printTasks();
}

void TODOApp::removeTask() {
  sqlite3_stmt *del;
  int taskNo = getTaskNumber();

  if (taskNo == -1)
    return;

  vector<int> ids = getIds();
  sqlite3_prepare_v2(db, "delete from task where id=?;", -1, &del, 0);
  sqlite3_bind_int(del, 1, ids[taskNo - 1]);
  sqlite3_step(del);
  sqlite3_finalize(del);

  printTasks();
}

void TODOApp::markTaskAsCompleted() {
  sqlite3_stmt *update;
  int taskNo = getTaskNumber();

  if (taskNo == -1)
    return;

  vector<int> ids = getIds();

  sqlite3_prepare_v2(db, "update task set completed=1 where id = ?;", -1,
                     &update, 0);
  sqlite3_bind_int(update, 1, ids[taskNo - 1]);
  sqlite3_step(update);
  sqlite3_finalize(update);

  printTasks();
}

void TODOApp::printTasks() {
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(db, "select * from task;", -1, &stmt, 0);
  int rc = sqlite3_step(stmt);
  int i = 1;
  cout << endl;
  while (rc != SQLITE_DONE) {
    cout << i << ". " << sqlite3_column_text(stmt, 2) << " **"
         << (sqlite3_column_int(stmt, 1) ? "completed**" : "pending**") << endl;
    i++;
    rc = sqlite3_step(stmt);
  }
  cout << endl;
  sqlite3_finalize(stmt);
}

vector<int> TODOApp::getIds() {
  vector<int> ids;
  sqlite3_stmt *select;

  sqlite3_prepare_v2(db, "select id from task;", -1, &select, 0);
  int rc = sqlite3_step(select);
  int count = 0;
  while (rc != SQLITE_DONE) {
    ids.push_back(sqlite3_column_int(select, 0));
    count++;
    rc = sqlite3_step(select);
  }

  sqlite3_finalize(select);
  return ids;
}

int TODOApp::getCount() {
  int count = 0;
  sqlite3_stmt *select;

  sqlite3_prepare_v2(db, "select count(*) from task;", -1, &select, 0);
  sqlite3_step(select);
  count = sqlite3_column_int(select, 0);
  sqlite3_finalize(select);
  return count;
}

int TODOApp::getTaskNumber() {
  int idx;
  int count = getCount();

  cout << "Enter Task number[" << (count ? 1 : 0) << "-" << count << "]: ";
  cin >> idx;

  if (idx < 1 || idx > count) {
    cout << "Invalid Task Number!" << endl;
    return -1;
  }
  return idx;
}
