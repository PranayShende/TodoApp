#include <sqlite3.h>
#include <vector>

#ifndef TODO_APP
#define TODO_APP

class TODOApp {
public:
  TODOApp();
  ~TODOApp();
  void run();
  void addTask();
  void removeTask();
  void markTaskAsCompleted();
  void printTasks();

private:
  std::vector<int> getIds();
  int getCount();
  int getTaskNumber();

  const char *const db_filename = "todo.db";
  sqlite3 *db;
  bool running;
  int availableKey;
};

#endif