#pragma once

#include <string>
#include <vector>

using namespace std;

class HelperFunc
{
public:
	static bool textContainsOnlySmallLetters(const string& text);
	static void printDashLine();
	static vector<double> generateNormalSizeDistribution(int maxSize, double mean = -1, double stddev = -1);
};

