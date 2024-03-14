#include <iostream>
#include <map>
#include <stdexcept>

#include "Connection.h"

using namespace std;

MysqlConnection::MysqlConnection()
{
	this->conn = mysql_init(0);
}

void MysqlConnection::connectToDb(string url, string user, string password, string dbName, int port)
{
	conn = mysql_real_connect(this->conn, url.c_str(), user.c_str(), password.c_str(), dbName.c_str(), port, NULL, 0);
	if (!conn) {
		cout << "Conection failed" << endl;
	}
	else {
		cout << "Connection successfull" << endl;
	}
}

vector<string> MysqlConnection::getColNames(string db, string table) const
{
	vector<string> colNames;
	int qstate;
	string query = "SELECT column_name as \'Column name\' FROM information_schema.COLUMNS WHERE table_schema = \'" + db + "\' AND table_name = \'" + table + "\' AND NOT COLUMN_KEY = \'PRI\';";
	qstate = mysql_query(this->conn, query.c_str());

	if (!qstate) {
		MYSQL_RES * res;
		MYSQL_ROW row;
		
		res = mysql_store_result(conn);
		while (row = mysql_fetch_row(res)) {
			colNames.push_back(row[0]);
		}
	}
	else {
		cout << "Query failed" << endl;
		throw runtime_error("Query failed");
	}

	return colNames;
}

map<string, double> MysqlConnection::getRow(string dbName, string tableName, const int& rowIdx) const
{
	map<string, double> rowMap;
	int qstate;
	string query = "SELECT * FROM " + tableName + " WHERE idx = " + to_string(rowIdx) + ";";
	vector<string> colNames = this->getColNames(dbName, tableName);
	qstate = mysql_query(this->conn, query.c_str());
	if (!qstate) {
		MYSQL_RES* res;
		MYSQL_ROW row;

		res = mysql_store_result(conn);
		row = mysql_fetch_row(res);

		for (int i = 0; i < colNames.size(); i++) {
			rowMap[colNames[i]] = stod(row[i+1]); // i+1 cause first columns is primary index
		}

	}
	else {
		cout << "Query failed" << endl;
		cout << "Query: " << query << endl;
		throw runtime_error("Query failed");
	}
	return rowMap;
}

vector<pair<int, double>> MysqlConnection::getTargetVarValues(string targetVarName, string primaryKeyName, string tableName) const
{
	vector<pair<int, double>> targetVarValues;
	int qstate;
	string query = "SELECT " + primaryKeyName + ", " + targetVarName + " FROM " + tableName;
	qstate = mysql_query(this->conn, query.c_str());
	if (!qstate) {
		MYSQL_RES* res;
		MYSQL_ROW row;
		res = mysql_store_result(conn);

		while (row = mysql_fetch_row(res)) {
			targetVarValues.push_back(make_pair<int, double>(stoi(row[0]), stod(row[1])));
		}
	}
	else {
		cout << "Query failed" << endl;
		throw runtime_error("Query failed");
	}
	return targetVarValues;
}

vector<int> MysqlConnection::getPrimaryKeys(string primaryKeyName, string tableName) const
{
	vector<int> primaryKeys;
	int qstate;
	string query = "SELECT " + primaryKeyName +  " FROM " + tableName;
	qstate = mysql_query(this->conn, query.c_str());
	if (!qstate) {
		MYSQL_RES* res;
		MYSQL_ROW row;
		res = mysql_store_result(conn);

		while (row = mysql_fetch_row(res)) {
			primaryKeys.push_back(stoi(row[0]));
		}
	}
	else {
		cout << "Query failed" << endl;
		throw runtime_error("Query failed");
	}
	return primaryKeys;
}

Connection::Connection()
{
}
