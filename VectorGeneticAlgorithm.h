#pragma once

#include "Fitness.h"
#include "Individual.h"

using namespace std;

class VectorGeneticAlgorithm
{
private:
	int size;
	double maxTime;
	double maxValue;
	double minValue;
	bool realNumbers;

	int populationSize;
	double newIndividualRatio;
	
	shared_ptr<Connection> connection;
	string dbName;
	string tableName;
	string target;
	string primaryKey;

	unique_ptr<FitnessFunction> fitnessFunc;
	double crossoverProb;
	double mutationProb;
	int tournamentSize;
	double randomIndividualProb;

	bool dbInMemory;
	shared_ptr<map<int, map<string, double>>> dbTablePtr;

public:
	VectorGeneticAlgorithm();
	vector<double> run(Individual & individual, vector<double> & originalConstants);
	void initializePopulation(vector<vector<double>> & population, vector<double> & originalConstants);


	void setGeneralParams(const int& size, const double& maxTime, const double& maxValue, const double& minValue, const bool& realNumbers);
	void setPopulationParams(const int& populationSize, const double& newIndividualRatio);
	void setConnectionParams(shared_ptr<Connection> conn, string dbName, string tableName, string primaryKey, string target);
	void setAlgParams(unique_ptr<FitnessFunction> fitnessFunc, const double & crossoverProb, const double & mutationProb,
						const int & tournamentSize, const double & randomIndividualProb);
	void setDbInMemory(bool dbInMemory, shared_ptr<map<int, map<string, double>>> dbTablePtr);

	vector<double> createRandomVector();
	double evaluateVector(const vector<double>& arr, Individual & individual, const vector<pair<int, double>>& targetValues);

	vector<double> onePointCrossover(const vector<double>& a, const vector<double>& b);
	int selectIndexTournament(const vector<double> & scores);
	void randomNumberMutation(vector<double>& arr);
};

