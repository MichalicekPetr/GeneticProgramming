#include <chrono>
#include <iostream> 
#include <limits>

#include "Individual.h"
#include "Random.h"
#include "VectorGeneticAlgorithm.h"

using namespace std;

VectorGeneticAlgorithm::VectorGeneticAlgorithm()
{
	this->maxTime = 0.0;
	this->size = 0;
	this->maxValue = 0.0;
	this->minValue = 0.0;
	this->realNumbers = false;
	this->populationSize = 0;
	this->newIndividualRatio = 0.0;
	this->connection = nullptr;
	this->dbName = "";
	this->tableName = "";
	this->target = "";
	this->primaryKey = "";
	this->fitnessFunc = nullptr;
	this->crossoverProb = 0.0;
	this->mutationProb = 0.0;
	this->tournamentSize = 0;
	this->randomIndividualProb = 0.0;
	this->dbInMemory = false;
	this->dbTablePtr = nullptr;
}

vector<double> VectorGeneticAlgorithm::run(Individual& individual, vector<double>& originalConstants)
{
	vector<pair<int, double>> targetValues = this->connection->getTargetVarValues(this->target, this->primaryKey, this->tableName);
	
	chrono::steady_clock::time_point beginTime = chrono::steady_clock::now();
	vector<double> bestIndividual(0);
	double maxFitness = - numeric_limits<double>::infinity();
	
	vector<vector<double>> population(0);
	initializePopulation(population, originalConstants);
	int generationNum = 0;

	while (true) {
		generationNum++;

		// Check time
		chrono::steady_clock::time_point currentTime = chrono::steady_clock::now();
		double elapsedTime = chrono::duration_cast<std::chrono::milliseconds> (currentTime - beginTime).count();
		elapsedTime /= 1000;
		if (elapsedTime > this->maxTime) {
			//cout << "Generations: " << generationNum << endl;
			break;
		}

		// Measure fitness
		vector<double> scores(0);
		for (int i = 0; i < this->populationSize; i++) {
			
			vector<double>& arr = population.at(i);

			double fitness = evaluateVector(arr, individual, targetValues);
			if ((fitness > maxFitness) || (maxFitness == - numeric_limits<double>::infinity())) {
				bestIndividual = population.at(i);
				maxFitness = fitness;
			}
			scores.push_back(fitness);
		}

		if ((maxFitness == -0) || (maxFitness == 0)) {
			break;
		}

		//Creating new population
		vector<vector<double>> newPopulation(0);
		while (newPopulation.size() < this->populationSize) {
			double seed1 = Random::randProb();
			double seed2 = Random::randProb();
			if (seed1 < this->randomIndividualProb) {
				newPopulation.push_back(this->createRandomVector());
			}
			else {
				if (seed2 < this->crossoverProb) {
					// Crossover
					int idx1 = selectIndexTournament(scores);
					int idx2 = selectIndexTournament(scores);
					newPopulation.push_back(onePointCrossover(population.at(idx1), population.at(idx2)));
				}
				else {
					// Select individual from old population
					int idx = selectIndexTournament(scores);
					newPopulation.push_back(population.at(idx));
				}
			}
		}

		// Mutation
		for (auto& arr : newPopulation) {
			this->randomNumberMutation(arr);
		}

		population = newPopulation;
	}

	return bestIndividual;
}

void VectorGeneticAlgorithm::initializePopulation(vector<vector<double>>& population, vector<double>& originalConstants)
{
	// Adding origin individuals to population
	if (originalConstants.size() > 0) {
		int count = floor(this->populationSize * this->newIndividualRatio);
		for (int i = 0; i < count; i++) {
			population.push_back(createRandomVector());
		}

		while (population.size() < this->populationSize) {
			vector<double> copy(originalConstants);
			population.push_back(copy);
		}
	}
	else {
		while (population.size() < this->populationSize) {
			population.push_back(createRandomVector());
		}
	}
}

void VectorGeneticAlgorithm::setGeneralParams(const int& size, const double& maxTime, const double& maxValue, const double& minValue, const bool& realNumbers)
{
	this->size = size;
	this->maxTime = maxTime;
	this->maxValue = maxValue;
	this->minValue = minValue;
	this->realNumbers = realNumbers;
}

void VectorGeneticAlgorithm::setPopulationParams(const int& populationSize, const double& newIndividualRatio)
{
	this->populationSize = populationSize;
	this->newIndividualRatio = newIndividualRatio;
}

void VectorGeneticAlgorithm::setConnectionParams(shared_ptr<Connection> conn, string dbName, string tableName, string primaryKey, string target)
{
	this->connection = conn;
	this->dbName = dbName;
	this->tableName = tableName;
	this->primaryKey = primaryKey;
	this->target = target;
}

void VectorGeneticAlgorithm::setAlgParams(unique_ptr<FitnessFunction> fitnessFunc, const double& crossoverProb, const double& mutationProb, const int& tournamentSize, const double& randomIndividualProb)
{
	this->fitnessFunc = move(fitnessFunc);
	this->crossoverProb = crossoverProb;
	this->mutationProb = mutationProb;
	this->tournamentSize = tournamentSize;
	this->randomIndividualProb = randomIndividualProb;
}

void VectorGeneticAlgorithm::setDbInMemory(bool dbInMemory, shared_ptr<map<int, map<string, double>>> dbTablePtr)
{
	this->dbInMemory = dbInMemory;
	this->dbTablePtr = dbTablePtr;
}

vector<double> VectorGeneticAlgorithm::createRandomVector()
{
	vector<double> arr(0);
	for (int i = 0; i < this->size; i++) {
		if(this->realNumbers){ 
			arr.push_back(Random::rand(this->minValue, this->maxValue));
		}
		else {
			arr.push_back(Random::randInt(this->minValue, this->maxValue));
		}
	}
	return arr;
}

double VectorGeneticAlgorithm::evaluateVector(const vector<double>& arr, Individual& individual, const vector<pair<int,double>> & targetValues)
{
	individual.getConstantTableRef().setTable(arr);
	if (this->dbInMemory) {
		return this->fitnessFunc->evaluate(individual, this->dbTablePtr, targetValues);
	}
	else {
		return this->fitnessFunc->evaluate(individual, this->connection, this->dbName, this->tableName, targetValues);
	}
}

vector<double> VectorGeneticAlgorithm::onePointCrossover(const vector<double>& a, const vector<double>& b)
{
	vector<double> offSpring(0);
	int crossoverPoint = Random::randInt(0, this->size);

	for (int i = 0; i < crossoverPoint; i++) {
		offSpring.push_back(a[i]);
	}
	for (int i = crossoverPoint; i < this->size; i++) {
		offSpring.push_back(b[i]);
	}

	return offSpring;
}

int VectorGeneticAlgorithm::selectIndexTournament(const vector<double> & scores)
{
	vector<int> idxs = Random::randInts(0, this->populationSize - 1, this->tournamentSize);
	int selectedIdx = idxs[0];
	double bestScore = scores.at(selectedIdx);

	for (const auto& idx : idxs) {
		if (scores.at(idx) > bestScore) {
			selectedIdx = idx;
			bestScore = scores.at(selectedIdx);

		}
	}

	return selectedIdx;
}

void VectorGeneticAlgorithm::randomNumberMutation(vector<double>& arr)
{
	for (int i = 0; i < this->size; i++) {
		double seed = Random::randProb();
		if (seed < this->mutationProb) {
			if (this->realNumbers) {
				arr[i] = Random::rand(this->minValue, this->maxValue);
			}
			else {
				arr[i] = Random::randInt(this->minValue, this->maxValue);
			}
		}
	}
}

