#include <iostream>
#include <string>

#include "DatabaseScripts.h"

using namespace std;

void DatabaseScripts::createRowInsertScriptTestDb1()
{
}

void DatabaseScripts::createTableInsertScriptTestDb1()
{
	string query = "CREATE TABLE \'testschema\'.\'testdb1 ("
		"\'idx\' INT NOT NULL,"
		"\'x1\' DECIMAL(5) NOT NULL,"
		"\'x2\' DECIMAL(5) NOT NULL,"
		"\'x3\' DECIMAL(5) NOT NULL,"
		"\'y\' DECIMAL(5) NOT NULL,"
		"PRIMARY KEY (\'idx\'));";
	cout << query << endl;
}
