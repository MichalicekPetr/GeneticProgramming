#include "assert.h"
#include <iostream>

#include "Test.h"
#include "Terminal.h"
#include "HelperFunc.h"

#include <mysql.h>



using namespace std;

bool Test::mutationTests()
{

	return false;
}

bool Test::mysqlDatabaseConnectionTest()
{
	MYSQL* conn;
	MYSQL_ROW row;
	MYSQL_RES res;
	
	conn = mysql_init(0);
	conn = mysql_real_connect(conn, "localhost", "root", "krtek", "testdb", 3306, NULL, 0);

	if (conn) {
		puts("Successfull connection to database");
	}
	else {
		puts("Connection to database failed");
	}
	return 0;
}

void Test::runAllTests()
{

}
