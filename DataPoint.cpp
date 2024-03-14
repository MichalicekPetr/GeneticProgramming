#include <string>

#include "DataPoint.h"

using namespace std;

DataPoint::DataPoint()
{
    this->colName = "";
}

DataPoint::DataPoint(string colName)
{
    this->colName = colName;
}

DataPoint::DataPoint(const DataPoint& dataPoint)
{
    this->colName = dataPoint.colName;
}

string DataPoint::getName() const
{
    return this->colName;
}
