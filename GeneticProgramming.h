#pragma once


#include <memory>

#include "Connection.h"
#include "Crossover.h"
#include "Fitness.h"
#include "Function.h"
#include "Mutation.h"
#include "Population.h"
#include "Selection.h"
#include "Terminal.h"

class GeneticProgramming
{
private:
	FunctionSet functionSet;
	TerminalSet terminalSet;
	Population population;
	unique_ptr<Mutation> mutation;

	unique_ptr<Crossover> crossover;
	double crossover_prob;

	unique_ptr<Selection> selection;

	unique_ptr<FitnessFunction> fitnessFunc;

	shared_ptr<Connection> connection;
	string dbName;
	string tableName;
	string target;
	string primaryKey;

	string user;
	string password;
	int port;
	string url;

	double randomIndividualProb;

	bool constantTuning;
	double constantTuningMaxTime;

	double vectorGA_crossoverProb;
	double vectorGA_mutationProb;
	int vectorGA_tournamentSize;
	double vectorGA_randomIndividualProb;
	int vectorGA_populationSize;
	double vectorGA_newIndividualRatio;

	bool saveDbToMemory;

public:
	GeneticProgramming();
	
	void standartRun(const int& maxGenerationNum = -1, const int& startTreeDepth = -1);

	void setPopulation(Population population);
	void setFunctionSet(FunctionSet functionSet);
	void setTerminalSet(TerminalSet terminalSet);
	void setMutation(unique_ptr<Mutation> mutation);
	void setSelection(unique_ptr<Selection> selection);
	void setCrossover(unique_ptr<Crossover> crossover, const double & crossoverProb);
	void setFitness(unique_ptr<FitnessFunction> fitnessFunc);
	void setDbThings(shared_ptr<Connection> conn, string dbName, string tableName, string primaryKey, bool saveDbToMemory);
	void setTarget(string target);
	void setLoginParams(string url, string user, string password, int port);
	void setRandomIndividualProb(const double& prob);
	void setTuneConstants(const bool& tuneConstants, const double& tuneConstantsMaxTime = 0.0);
	void setVectorGAParams(const double & crossoverProb, const double & mutationProb, const int & tournamentSize,
		const double & randomIndividualProb, const int & populationSize, const double & newIndividualRatio);

	vector<double> tuneConstants(Individual & individual, vector<double> originalConstants, shared_ptr<map<int, map<string, double>>> dbTablePtr = nullptr);

	shared_ptr<map<int, map<string, double>>> saveDbTableInMemory();
	
};


