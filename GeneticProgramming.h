#pragma once


#include <memory>
#include <omp.h>

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

	vector<shared_ptr<Connection>> connections;
	string dbName;
	string tableName;
	string target;
	string primaryKey;

	string user;
	string password;
	int port;
	string url;
	bool saveDbToMemory;

	double randomIndividualProb;

	bool constantTuning;
	double constantTuningMaxTime;

	double vectorGA_crossoverProb;
	double vectorGA_mutationProb;
	int vectorGA_tournamentSize;
	double vectorGA_randomIndividualProb;
	int vectorGA_populationSize;
	double vectorGA_newIndividualRatio;

	bool datFile;
	string GPGAdataFolderPath;
	string GPdataFolderPath;

	bool useWindow;
	int windowHeight;
	int windowWidth;
	
	int maxTreeDepth;
	int threadCnt;

	bool mergeConstantOptimalization;
	bool removeUselessBranchesOptimalization;
	bool DAGOptimalization;

public:
	GeneticProgramming();
	
	void standartRun(const int& maxGenerationNum = -1, const int& startTreeDepth = -1, bool debugPrints = true);

	void setPopulation(Population population);
	void setFunctionSet(FunctionSet functionSet);
	void setTerminalSet(TerminalSet terminalSet);
	void setMutation(unique_ptr<Mutation> mutation);
	void setSelection(unique_ptr<Selection> selection);
	void setCrossover(unique_ptr<Crossover> crossover, const double & crossoverProb);
	void setFitness(unique_ptr<FitnessFunction> fitnessFunc);
	void initConnections(std::shared_ptr<Connection> prototype, int threadCount, std::string dbName, std::string tableName, std::string primaryKey, bool saveDbToMemory);
	void setTarget(string target);
	void setLoginParams(string url, string user, string password, int port);
	void setRandomIndividualProb(const double& prob);
	void setTuneConstants(const bool& tuneConstants, const double& tuneConstantsMaxTime = 0.0);
	void setVectorGAParams(const double & crossoverProb, const double & mutationProb, const int & tournamentSize,
		const double & randomIndividualProb, const int & populationSize, const double & newIndividualRatio);
	void setOutputFileParams(bool datFile, string GPdataFolderPath, string GPGAdataFolderPath);
	void setWindowParams(bool useWindow, int windowHeight, int windowWidth);
	void setMaxDepth(const int& maxDepth);
	void setThreadCnt(const int& threadCnt);
	void setOptimalizationParams(bool mergeConstantOptimalization, bool removeUselessBranchesOptimalization, bool DAGOptimalization);

	vector<double> tuneConstants(Individual & individual, vector<double> originalConstants, shared_ptr<map<int, map<string, double>>> dbTablePtr = nullptr);

	shared_ptr<map<int, map<string, double>>> saveDbTableInMemory();
	shared_ptr<map<int, map<string, double>>> saveDbTableInMemory(const vector<int>& primaryKeys, const vector<string>& colNames);
	shared_ptr<map<int, map<string, double>>> createWindow(vector<pair<int, double>> & targetValues);

	string createFileName() const;

	shared_ptr<Connection> getConnectionForThread(int threadIdx);
};


