#pragma once
#include <vector>

using namespace std;

class Random
{
public:
	static int randInt(const size_t & min, const size_t & max);
	static double randProb();
	static double rand(const int& min, const int& max);
	static vector<int> randInts(const size_t& min, const size_t& max, const size_t& count);
};

