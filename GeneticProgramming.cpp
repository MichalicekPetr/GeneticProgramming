#include <chrono>
#include <iostream>
#include <limits>

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
}

void GeneticProgramming::standartRun(const int & maxGenerationNum, const int & startTreeDepth)
{

	connection->connectToDb(this->url, this->user, this->password, this->dbName, this->port);
	shared_ptr<map<int, map<string, double>>> dbMapPtr;
	vector<pair<int, double>> targetValues(0);
	if (this->saveDbToMemory) {
		dbMapPtr = this->saveDbTableInMemory();
		targetValues = this->connection->getTargetVarValues(this->target, this->primaryKey, this->tableName);
	}
	
	int generationNum = 0;
	int populationSize = this->population.getSize();
	this->population.initPopulation(startTreeDepth, this->functionSet, this->terminalSet);
	FitnessFunction * fitness = this->fitnessFunc.get();
	Individual bestOfBest;
	double bestScore = - numeric_limits<double>::infinity();

	while (true) {
		generationNum++;
		if (generationNum > maxGenerationNum) {
			break;
		}

		cout << "---------------------- Generation n." << generationNum << " ----------------------" << endl;

		if (this->constantTuning) {
			for (int i = 0; i < this->population.getSize(); i++) {

				Individual & individualRef = population.at(i);
				double scoreBefore = fitness->evaluate(population.at(i), this->connection, this->dbName, this->tableName, this->target, this->primaryKey);
				if (individualRef.hasConstantTable()) {
					vector<double> constants = this->tuneConstants(individualRef, individualRef.getConstantTableRef().getTable(), dbMapPtr);
					population.at(i).getConstantTableRef().setTable(constants);
				}
				else {
					population.at(i).createConstantTable();
					vector<double> constants = this->tuneConstants(population.at(i), vector<double>(0), dbMapPtr);
					population.at(i).getConstantTableRef().setTable(constants);
				}

				double scoreAfter = fitness->evaluate(population.at(i), this->connection, this->dbName, this->tableName, this->target, this->primaryKey);
				cout << "Before: " << scoreBefore << "; After: " << scoreAfter << endl;
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
		cout << "Average fitness: " << acc / (populationSize - infCnt) << endl;
		cout << "Average depth: " << depthAcc / (populationSize - infCnt) << endl;
		cout << "Best fitness: " << maxFitness << endl;
		cout << "Best individual: " << endl << this->population.at(bestIndividualIdx) << endl;

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
			}
			else{
				double seed2 = Random::randProb();
				if (seed2 <= this->crossover_prob) {
					Individual & parent1 = this->selection->selectIndividual(this->population, fitnessValues);
					Individual & parent2 = this->selection->selectIndividual(this->population, fitnessValues);
					newIndividual = this->crossover->createOffspring(parent1, parent2);
				}
				else {
					Individual & selected = this->selection->selectIndividual(this->population, fitnessValues);
					newIndividual = Individual(selected);
				}
			}

			this->mutation->mutate(newIndividual);
			newPopulation.push_back(newIndividual);
			newPopulationSize++;
		}

		this->population.setPopulation(newPopulation);
	}

	cout << "End of genetic programming" << endl;
	cout << "Best fitness: " << bestScore << endl;
	cout << "Best individual (depth: " << bestOfBest.getMaxDepth() << "): " << endl << bestOfBest << endl;
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


