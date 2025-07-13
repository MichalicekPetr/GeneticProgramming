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
	virtual vector<string> getColNamesWithoutTargetAndPrimaryKey(string db, string table, string target, string primaryKey) const = 0;
	virtual map<string, double> getRow(string dbName, string tableName, const int& rowIdx) const = 0;
	virtual map<string, double> getRow(string dbName, string tableName, const int& rowIdx, const vector<string>& colNames) const = 0;
	virtual vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName) const = 0;
	virtual vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName, const vector<int>& rowIdxs) const = 0;
	virtual vector<int> getPrimaryKeys(string primaryKeyName, string tableName) const = 0;
	virtual bool isConnectedToDb() const = 0;
};

class MysqlConnection : public Connection {
private:
	MYSQL* conn;
	bool connectedToDb;

public:
	MysqlConnection();
	void connectToDb(string url, string user, string password, string dbName, int port) override;
	vector<string> getColNames(string db, string table) const override;
	vector<string> getColNamesWithoutTargetAndPrimaryKey(string db, string table, string target, string primaryKey) const override;
	map<string, double> getRow(string dbName, string tableName, const int& rowIdx) const override;
	map<string, double> getRow(string dbName, string tableName, const int& rowIdx, const vector<string>& colNames) const override;
	vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName) const override;
	vector<pair<int, double>> getTargetVarValues(string targetVarName, string primaryKeyName, string tableName, const vector<int> & rowIdxs) const override;
	vector<int> getPrimaryKeys(string primaryKeyName, string tableName) const override;
	void close();
	bool isConnectedToDb() const override; 
};

