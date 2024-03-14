#include <chrono>
#include <iostream>
#include <random>
#include <set>

#include "Random.h"

using namespace std;

int Random::randInt(const size_t & min, const size_t & max)
{
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> distribution(min, max);
	return distribution(gen);
}

double Random::randProb()
{
    std::mt19937_64 rng;
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
    std::uniform_real_distribution<double> unif(0, 1);
    return unif(rng);
}

double Random::rand(const int & min, const int & max)
{
    std::mt19937_64 rng;
    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
    rng.seed(ss);
    std::uniform_real_distribution<double> unif(min, max);
    return unif(rng);
}

vector<int> Random::randInts(const size_t& min, const size_t& max, const size_t& count)
{
    vector<int> nums(0);
    set<int> numsSet;
    int generatedNums = 0;

    while (generatedNums < count) {
        int seed = Random::randInt(min, max);
        if(numsSet.find(seed) == numsSet.end() || numsSet.empty()){
            numsSet.insert(seed);
            nums.push_back(seed);
            generatedNums++;
        }
    }
    
    return nums;
}
