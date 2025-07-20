#include <ctime>
#include <fstream>
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "GeneticProgramming.h"
#include "Population.h"
#include "Random.h"
#include "VectorGeneticAlgorithm.h"

using namespace std;

GeneticProgramming::GeneticProgramming()
{
	this->population = Population();
	this->functionSet = FunctionSet();
	this->terminalSet = TerminalSet();
	this->mutation = nullptr;
	this->selection = nullptr;
	this->crossover = nullptr;
	this->crossover_prob = 0;
	this->connection = nullptr;
	this->dbName = "";
	this->tableName = "";
	this->primaryKey = "";
	this->target = "";
	this->randomIndividualProb = 0;
	this->constantTuning = false;
	this->constantTuningMaxTime = 0.0;
	this->vectorGA_crossoverProb = 0.0;
	this->vectorGA_mutationProb = 0.0;
	this->vectorGA_tournamentSize = 0;
	this->vectorGA_randomIndividualProb = 0.0;
	this->vectorGA_populationSize = 0;
	this->vectorGA_newIndividualRatio = 0.0;
	this->saveDbToMemory = false;
	this->datFile = false;
	this->port = 0;
	this->useWindow = false;
	this->windowHeight = 0;
	this->windowWidth = 0;
	this->maxTreeDepth = 10;
}

void GeneticProgramming::standartRun(const int & maxGenerationNum, const int & startTreeDepth, bool debugPrints)
{
	if (!this->connection->isConnectedToDb()) {
		connection->connectToDb(this->url, this->user, this->password, this->dbName, this->port);
	}


	shared_ptr<map<int, map<string, double>>> dbMapPtr;
	vector<pair<int, double>> targetValues(0);
	
	if (this->useWindow) {
		dbMapPtr = this->createWindow(targetValues);
		this->saveDbToMemory = true;
	}
	else {
		if (this->saveDbToMemory) {
			cout << "Saving db to memory start" << endl;
			dbMapPtr = this->saveDbTableInMemory();
			targetValues = this->connection->getTargetVarValues(this->target, this->primaryKey, this->tableName);
			cout << "Saving db to memory end" << endl;
		}
	}

	cout << "Population inicialization start" << endl;
	int generationNum = 0;
	int populationSize = this->population.getSize();
	this->population.initPopulation(startTreeDepth, this->functionSet, this->terminalSet);
	FitnessFunction * fitness = this->fitnessFunc.get();
	Individual bestOfBest;
	double bestScore = - numeric_limits<double>::infinity();
	cout << "Population inicialization end" << endl;
	
	ofstream file;
	if (this->datFile) {
		string fileName = this->createFileName();
		file = ofstream(fileName);
	}

	while (true) {
		generationNum++;
		if (generationNum > maxGenerationNum) {
			break;
		}

		if (debugPrints) {
			cout << "---------------------- Generation n." << generationNum << " ----------------------" << endl;
		}
		
		double accImp = 0.0;
		if (this->constantTuning) {
			
			for (int i = 0; i < this->population.getSize(); i++) {
				double improvementAcc = 0;
				Individual & individualRef = population.at(i);
				if (debugPrints) {
					cout << "Individual n." << i + 1 << endl << individualRef << endl;
				}
				double scoreBefore = fitness->evaluate(population.at(i), dbMapPtr, targetValues);
				if (individualRef.hasConstantTable()) {
					if (debugPrints) {
						cout << "Constant table: " << endl;
						individualRef.getConstantTableRef().debugPrint();
					}
					vector<double> constants = this->tuneConstants(individualRef, individualRef.getConstantTableRef().getTable(), dbMapPtr);
					if (debugPrints) {
						cout << "Constant table: " << endl;
						individualRef.getConstantTableRef().debugPrint();
					}
					population.at(i).getConstantTableRef().setTable(constants);
				}
				else {
					population.at(i).createConstantTable();
					if (debugPrints) {
						cout << "Constant table: " << endl;
						individualRef.getConstantTableRef().debugPrint();
					}
					vector<double> constants = this->tuneConstants(population.at(i), vector<double>(0), dbMapPtr);
					if (debugPrints) {
						cout << "Constant table: " << endl;
						individualRef.getConstantTableRef().debugPrint();
					}
					population.at(i).getConstantTableRef().setTable(constants);
				}
				if (debugPrints) {
					cout << "Individual n." << i + 1 << " after constant tuning" << endl << individualRef << endl;
				}

				double scoreAfter = fitness->evaluate(population.at(i), dbMapPtr, targetValues);
				cout << "Before: " << scoreBefore << "; After: " << scoreAfter << endl;
				accImp += min(100, -((scoreBefore + scoreAfter) / scoreBefore));
			}
		}

		double acc = 0; double depthAcc = 0;
		double maxFitness;
		int bestIndividualIdx;
		bool maxFitnessSet = false;
		int infCnt = 0;
		vector <double> fitnessValues = vector<double>(0);
		for (int i = 0; i < this->population.getSize(); i++) {
			Individual & current = population.at(i);
			double score;
			if (this->saveDbToMemory) {
				score = fitness->evaluate(current, dbMapPtr, targetValues);
			}
			else{
				score = fitness->evaluate(current, this->connection, this->dbName, this->tableName, this->target, this->primaryKey);
			}
			fitnessValues.push_back(score);

			if (!isinf(score)) {
				acc += score;
				depthAcc += current.getMaxDepth();
			}
			else {
				infCnt++;
			}
			
			if (maxFitnessSet) {
				if (score > maxFitness) {
					bestIndividualIdx = i;
					maxFitness = score;
					if (maxFitness >= bestScore) {
						bestScore = maxFitness;
						bestOfBest = population.at(bestIndividualIdx);
					}
				}
			}
			else{
				bestIndividualIdx = i;
				maxFitness = score;
				maxFitnessSet = true;
				if (maxFitness >= bestScore) {
					bestScore = maxFitness;
					bestOfBest = population.at(bestIndividualIdx);
				}
			}
		}

		if(debugPrints){
			cout << "Average fitness: " << acc / (populationSize - infCnt) << endl;
			cout << "Average depth: " << depthAcc / (populationSize - infCnt) << endl;
			cout << "% of improvements: " << accImp / this->vectorGA_populationSize;
			cout << "Best fitness: " << maxFitness << endl;
			cout << "Best individual: " << endl << this->population.at(bestIndividualIdx) << endl;
		}


		if (this->datFile) {
			file << generationNum << " " << acc / (populationSize - infCnt) << " " << maxFitness << endl;
		}

		if ((bestScore == - 0) || (bestScore == 0)) {
			break;
		}

		vector<Individual> newPopulation(0);
		int newPopulationSize = 0;
		while (newPopulationSize < populationSize) {
			double seed1 = Random::randProb(); 
			Individual newIndividual;
			
			if (seed1 <= this->randomIndividualProb) { 
				newIndividual = Individual::generateRandomTreeGrowMethod(startTreeDepth, this->functionSet, this->terminalSet);
				if (debugPrints) {
					cout << "New individual created by random: " << endl << newIndividual << endl;
				}
			}
			else{
				double seed2 = Random::randProb();
				if (seed2 <= this->crossover_prob) {
					Individual & parent1 = this->selection->selectIndividual(this->population, fitnessValues);
					Individual & parent2 = this->selection->selectIndividual(this->population, fitnessValues);
					newIndividual = this->crossover->createOffspring(parent1, parent2, this->maxTreeDepth);
					if (debugPrints) {
						cout << "New individual created by crossover: " << endl << "Parent1:" << endl << parent1 << endl 
							<< "Parent2:" << parent2 << endl << "New Individual:" << endl << newIndividual << endl;
					}
				}
				else {
					Individual & selected = this->selection->selectIndividual(this->population, fitnessValues);
					newIndividual = Individual(selected);
					cout << "New individual created copying: " << endl << newIndividual << endl;
				}
			}

			this->mutation->mutate(newIndividual, this->maxTreeDepth);
			cout << "New individual after mutation:" << endl << newIndividual << endl;
			newPopulation.push_back(newIndividual);
			newPopulationSize++;
		}

		this->population.setPopulation(newPopulation);
	}

	if (debugPrints) {
		cout << "End of genetic programming" << endl;
		cout << "Best fitness: " << bestScore << endl;
		cout << "Best individual (depth: " << bestOfBest.getMaxDepth() << "): " << endl << bestOfBest << endl;
	}

	if (this->datFile) {
		file.close();
	}
} 

void GeneticProgramming::setPopulation(Population population)
{
	this->population = move(population);
}

void GeneticProgramming::setFunctionSet(FunctionSet functionSet)
{
	this->functionSet = functionSet;
}

void GeneticProgramming::setTerminalSet(TerminalSet terminalSet)
{
	this->terminalSet = terminalSet;
}

void GeneticProgramming::setMutation(unique_ptr<Mutation> mutation)
{
	this->mutation = move(mutation);
}

void GeneticProgramming::setSelection(unique_ptr<Selection> selection)
{
	this->selection = move(selection);
}

void GeneticProgramming::setCrossover(unique_ptr<Crossover> crossover, const double& crossoverProb)
{
	this->crossover = move(crossover);
	this->crossover_prob = crossoverProb;
}

void GeneticProgramming::setFitness(unique_ptr<FitnessFunction> fitnessFunc)
{
	this->fitnessFunc = move(fitnessFunc);
}

void GeneticProgramming::setDbThings(shared_ptr<Connection> conn, string dbName, string tableName, string primaryKey, bool saveDbToMemory)
{
	this->connection = move(conn);
	this->dbName = dbName;
	this->tableName = tableName;
	this->primaryKey = primaryKey;
	this->saveDbToMemory = saveDbToMemory;
}

void GeneticProgramming::setTarget(string target)
{
	this->target = target;
}

void GeneticProgramming::setLoginParams(string url, string user, string password, int port)
{
	this->url = url;
	this->user = user;
	this->password = password;
	this->port = port;
}

void GeneticProgramming::setRandomIndividualProb(const double& prob)
{
	this->randomIndividualProb = 0.02;
}

void GeneticProgramming::setTuneConstants(const bool& tuneConstants, const double& tuneConstantsMaxTime)
{
	this->constantTuning = tuneConstants;
	this->constantTuningMaxTime = tuneConstantsMaxTime;
}

void GeneticProgramming::setVectorGAParams(const double& crossoverProb, const double& mutationProb, const int& tournamentSize, const double& randomIndividualProb, const int& populationSize, const double& newIndividualRatio)
{
	this->vectorGA_crossoverProb = crossoverProb;
	this->vectorGA_mutationProb = mutationProb;
	this->vectorGA_tournamentSize = tournamentSize;
	this->vectorGA_newIndividualRatio = newIndividualRatio;
	this->vectorGA_randomIndividualProb = randomIndividualProb;
	this->vectorGA_populationSize = populationSize;
}

void GeneticProgramming::setOutputFileParams(bool datFile, string GPdataFolderPath, string GPGAdataFolderPath)
{
	this->datFile = datFile;
	this->GPdataFolderPath = GPdataFolderPath;
	this->GPGAdataFolderPath = GPGAdataFolderPath;
}

void GeneticProgramming::setWindowParams(bool useWindow, int windowHeight, int windowWidth)
{
	this->useWindow = useWindow;
	this->windowHeight = windowHeight;
	this->windowWidth = windowWidth;
}

void GeneticProgramming::setMaxDepth(const int& maxDepth)
{
	this->maxTreeDepth = maxDepth;
}

vector<double> GeneticProgramming::tuneConstants(Individual& individual, vector<double> originalConstants, shared_ptr<map<int, map<string, double>>> dbTablePtr)
{
	int size = individual.getConstantTableRef().getSize();

	if (size == 0) {	
		return vector<double>(0);
	}

	double time = this->constantTuningMaxTime;
	double valueMax = this->terminalSet.getMax();
	double valueMin = this->terminalSet.getMin();
	bool realNumbers = this->terminalSet.containsRealNumbers();

	VectorGeneticAlgorithm geneticAlgorithm = VectorGeneticAlgorithm();
	geneticAlgorithm.setConnectionParams(this->connection, this->dbName, this->tableName, this->primaryKey, this->target);
	geneticAlgorithm.setGeneralParams(size, time, valueMax, valueMin, realNumbers);
	geneticAlgorithm.setAlgParams(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()), this->vectorGA_crossoverProb,
		this->vectorGA_mutationProb, this->vectorGA_tournamentSize, vectorGA_randomIndividualProb);
	geneticAlgorithm.setPopulationParams(this->vectorGA_populationSize, this->vectorGA_newIndividualRatio);
	geneticAlgorithm.setDbInMemory(this->saveDbToMemory, dbTablePtr);

	return geneticAlgorithm.run(individual, originalConstants);
}

shared_ptr<map<int, map<string, double>>> GeneticProgramming::saveDbTableInMemory()
{
	shared_ptr<map<int, map<string, double>>> dbMapPtr(new map<int, map<string, double>>());
	vector<int> primaryKeys = this->connection->getPrimaryKeys(this->primaryKey, this->tableName);
	for (const auto& idx : primaryKeys) {
		map <string, double> rowMap = this->connection->getRow(this->dbName, this->tableName, idx);
		dbMapPtr->insert({idx, rowMap});
	}
	return dbMapPtr;
}

shared_ptr<map<int, map<string, double>>> GeneticProgramming::saveDbTableInMemory(const vector<int> & primaryKeys, const vector<string> & colNames)
{
	shared_ptr<map<int, map<string, double>>> dbMapPtr(new map<int, map<string, double>>());
	for (const auto& idx : primaryKeys) {
		map <string, double> rowMap = this->connection->getRow(this->dbName, this->tableName, idx, colNames);
		dbMapPtr->insert({ idx, rowMap });
	}
	return dbMapPtr;
}

shared_ptr<map<int, map<string, double>>> GeneticProgramming::createWindow(vector<pair<int, double>>& targetValues)
{
	vector<int> primaryKeys = this->connection->getPrimaryKeys(this->primaryKey, this->tableName);
	vector<string> colNames = this->connection->getColNamesWithoutTargetAndPrimaryKey(this->dbName, this->tableName,
		this->target, this->primaryKey);

	if (primaryKeys.size() < this->windowHeight) {
		cout << "Vìtší výška okna než poèet øádkù v tabulce" << endl;
		throw invalid_argument("");
	}

	if (colNames.size() < this->windowWidth) {
		cout << "Vìtší šírka okna než poèet sloupcù v tabulce" << endl;
		throw invalid_argument("");
	}

	vector<int> rowsIdxs = Random::randInts(0, primaryKeys.size() - 1, this->windowHeight);
	vector<int> colsIdxs = Random::randInts(0, colNames.size() - 1, this->windowWidth);
	vector<int> chosenPrimaryKeys(0);
	vector<string> chosenColNames(0);
	for (const auto& idx : rowsIdxs) {
		chosenPrimaryKeys.push_back(primaryKeys[idx]);
	}
	for (const auto& idx : colsIdxs) {
		chosenColNames.push_back(colNames[idx]);
	}

	this->terminalSet = TerminalSet(this->terminalSet.getMin(), this->terminalSet.getMax(), this->terminalSet.containsRealNumbers(), chosenColNames);
	this->mutation->setTerminalSet(TerminalSet(this->terminalSet.getMin(), this->terminalSet.getMax(), this->terminalSet.containsRealNumbers(), chosenColNames));
	vector<pair<int, double>> chosenTargetValues = this->connection->getTargetVarValues(this->target, this->primaryKey, this->tableName, chosenPrimaryKeys);
	for (auto x : chosenTargetValues) {
		targetValues.push_back(x);
	}

	return this->saveDbTableInMemory(chosenPrimaryKeys, chosenColNames);
}

string GeneticProgramming::createFileName() const
{
	time_t t = time(0);  
	tm now{};
	localtime_s(&now, &t);

	int year = now.tm_year + 1900;
	int month = now.tm_mon + 1;
	int day = now.tm_mday;
	int hour = now.tm_hour;
	int minutes = now.tm_min;
	int seconds = now.tm_sec;


	string fileName = "";
	if (this->constantTuning) {
		fileName = fileName + this->GPGAdataFolderPath + "GP+GA_";
	}
	else {
		fileName = fileName + this->GPdataFolderPath + "GP_";
	}
	fileName += to_string(year) + "_" + to_string(month) + "_" + to_string(day) + 
		"_" + to_string(hour) + "_" + to_string(minutes) + "_" + to_string(seconds) + ".dat";
	return fileName;
}


