#pragma once
#include <vector>
#include <string>
#include <map>
#include <mysql.h>

using namespace std;

class Connection
{
public:
	Connection();
	virtual void connectToDb(string url, string user, string password, string dbName, int port) = 0;
	virtual vector<string> getColNames(string db, string table) const = 0;
	virtual map<string, double> getRow(string dbName, string tableName, const int& rowIdx) const = 0;
	virtual vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName) const = 0;
	virtual vector<int> getPrimaryKeys(string primaryKeyName, string tableName) const = 0;
};

class MysqlConnection : public Connection {
private:
	MYSQL* conn;

public:
	MysqlConnection();
	void connectToDb(string url, string user, string password, string dbName, int port) override;
	vector<string> getColNames(string db, string table) const override;
	map<string, double> getRow(string dbName, string tableName, const int& rowIdx) const override;
	vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName) const override;
	vector<int> getPrimaryKeys(string primaryKeyName, string tableName) const override;
	void close();
};

