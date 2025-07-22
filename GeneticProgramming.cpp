#include <ctime>
#include <fstream>
#include <chrono>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <mutex>

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
	this->threadCnt = 1;
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
			dbMapPtr = this->saveDbTableInMemory();
			targetValues = this->connection->getTargetVarValues(this->target, this->primaryKey, this->tableName);
		}
	}

	int generationNum = 0;
	int populationSize = this->population.getSize();
	this->population.initPopulation(startTreeDepth, this->functionSet, this->terminalSet);
	FitnessFunction * fitness = this->fitnessFunc.get();
	Individual bestOfBest;
	double bestScore = - numeric_limits<double>::infinity();
	
	ofstream file;
	if (this->datFile) {
		string fileName = this->createFileName();
		file = ofstream(fileName);
	}

	while (true) {
		// Začátek měření času
		auto start = std::chrono::high_resolution_clock::now();
		generationNum++;
		if (generationNum > maxGenerationNum) {
			break;
		}

		if (debugPrints) {
			cout << "---------------------- Generation n." << generationNum << " ----------------------" << endl;
		}
		
		double accImp = 0.0;
		if (this->constantTuning) {
			
			

			// Nastavení počtu vláken
			omp_set_num_threads(this->threadCnt);

			// Použijeme reduction pro správný součet accImp
			#pragma omp parallel for reduction(+:accImp)
			for (int i = 0; i < this->population.getSize(); i++) {
				double improvementAcc = 0;
				Individual& individualRef = population.at(i);

				if (debugPrints) {
				#pragma omp critical
					{
						cout << "Individual n." << i + 1 << endl << individualRef << endl;
					}
				}

				double scoreBefore = fitness->evaluate(individualRef, dbMapPtr, targetValues);

				if (individualRef.hasConstantTable()) {
					if (debugPrints) {
					#pragma omp critical
						{
							cout << "Constant table: " << endl;
							individualRef.getConstantTableRef().debugPrint();
						}
					}

					vector<double> constants = this->tuneConstants(individualRef, individualRef.getConstantTableRef().getTable(), dbMapPtr);

					if (debugPrints) {
					#pragma omp critical
						{
							cout << "Constant table: " << endl;
							individualRef.getConstantTableRef().debugPrint();
						}
					}

					individualRef.getConstantTableRef().setTable(constants);
				}
				else {
					individualRef.createConstantTable();

					if (debugPrints) {
					#pragma omp critical
						{
							cout << "Constant table: " << endl;
							individualRef.getConstantTableRef().debugPrint();
						}
					}

					vector<double> constants = this->tuneConstants(individualRef, vector<double>(0), dbMapPtr);

					if (debugPrints) {
					#pragma omp critical
						{
							cout << "Constant table: " << endl;
							individualRef.getConstantTableRef().debugPrint();
						}
					}

					individualRef.getConstantTableRef().setTable(constants);
				}

				if (debugPrints) {
				#pragma omp critical
					{
						cout << "Individual n." << i + 1 << " after constant tuning" << endl << individualRef << endl;
					}
				}

				double scoreAfter = fitness->evaluate(individualRef, dbMapPtr, targetValues);

				if (debugPrints) {
					#pragma omp critical
					{
						cout << "Before: " << scoreBefore << "; After: " << scoreAfter << endl;
					}
				}

				accImp += min(100.0, -((scoreBefore + scoreAfter) / scoreBefore));
			}



			
		}

		omp_set_num_threads(this->threadCnt);

		// Inicializace
		double acc = 0;
		double depthAcc = 0;
		int infCnt = 0;

		int bestIndividualIdx = -1;
		double maxFitness = 0;
		bool maxFitnessSet = false;

		// Prealokujeme fitness hodnoty
		vector<double> fitnessValues(this->population.getSize());

		// Paralelní smyčka
		#pragma omp parallel
		{
			// Lokální proměnné pro každý thread
			double localAcc = 0;
			double localDepthAcc = 0;
			int localInfCnt = 0;

			int localBestIdx = -1;
			double localMaxFitness = 0;
			bool localMaxSet = false;

			#pragma omp for
			for (int i = 0; i < this->population.getSize(); i++) {
				Individual& current = population.at(i);
				double score;

				if (this->saveDbToMemory) {
					score = fitness->evaluate(current, dbMapPtr, targetValues);  // ← musí být thread-safe!
				}
				else {
					score = fitness->evaluate(current, this->connection, this->dbName, this->tableName, this->target, this->primaryKey);
				}

				fitnessValues[i] = score;

				if (!isinf(score)) {
					localAcc += score;
					localDepthAcc += current.getMaxDepth();
				}
				else {
					localInfCnt++;
				}

				if (localMaxSet) {
					if (score > localMaxFitness) {
						localMaxFitness = score;
						localBestIdx = i;
					}
				}
				else {
					localMaxFitness = score;
					localBestIdx = i;
					localMaxSet = true;
				}
			}

			// Redukce výsledků všech threadů
			#pragma omp critical
			{
				acc += localAcc;
				depthAcc += localDepthAcc;
				infCnt += localInfCnt;

				if (localMaxSet) {
					if (!maxFitnessSet || localMaxFitness > maxFitness) {
						maxFitness = localMaxFitness;
						bestIndividualIdx = localBestIdx;
						maxFitnessSet = true;

						if (maxFitness >= bestScore) {
							bestScore = maxFitness;
							bestOfBest = population.at(bestIndividualIdx);
						}
					}
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

		omp_set_num_threads(this->threadCnt);

		// Předalokujeme nový vektor s cílovou velikostí
		vector<Individual> newPopulation(populationSize);

		// Použijeme paralelní for – každý thread vytvoří jednoho jedince
		#pragma omp parallel for
		for (int i = 0; i < populationSize; i++) {
			cout << "Creating individual n." << i << endl;
			Individual newIndividual;
			double seed1 = Random::randProb();  // musí být thread-safe!

			if (seed1 <= this->randomIndividualProb) {
				cout << "Creating random individual" << endl;
				newIndividual = Individual::generateRandomTreeGrowMethod(startTreeDepth, this->functionSet, this->terminalSet);

				if (debugPrints) {
					#pragma omp critical
					cout << "New individual created by random: " << endl << newIndividual << endl;
				}
			}
			else {
				double seed2 = Random::randProb();
				if (seed2 <= this->crossover_prob) {
					cout << "Crossover start" << endl;
					// Výběr rodičů – musí být thread-safe, nebo to obalit critical
					Individual parent1, parent2;
					parent1 = this->selection->selectIndividual(this->population, fitnessValues);
					parent2 = this->selection->selectIndividual(this->population, fitnessValues);

					newIndividual = this->crossover->createOffspring(parent1, parent2, this->maxTreeDepth);

					if (debugPrints) {
					#pragma omp critical
						{
							cout << "New individual created by crossover: " << endl
								<< "Parent1:" << endl << parent1 << endl
								<< "Parent2:" << endl << parent2 << endl
								<< "New Individual:" << endl << newIndividual << endl;
						}
					}
					cout << "Crossover end" << endl;
				}
				else {
					cout << "Copying" << endl;
					Individual selected;
					{
						selected = this->selection->selectIndividual(this->population, fitnessValues);
					}

					newIndividual = Individual(selected);

					if (debugPrints) {
					#pragma omp critical
						{
							cout << "New individual created copying: " << endl << newIndividual << endl;
						}
					}
				}
			}

			// Mutace – může být critical, pokud není thread-safe
			this->mutation->mutate(newIndividual, this->maxTreeDepth);

			if (debugPrints) {
			#pragma omp critical
				cout << "New individual after mutation:" << endl << newIndividual << endl;
			}

			// Uložení na správnou pozici (už je přiděleno)
			newPopulation[i] = newIndividual;
		}


		// Konec měření času
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		this->population.setPopulation(newPopulation);
		cout << "[DEBUG] Generation took " << duration << " ms with " << this->threadCnt << " threads." << endl;
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

void GeneticProgramming::setThreadCnt(const int& threadCnt)
{
	this->threadCnt = threadCnt;
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
		cout << "Větší výška okna než počet řádků v tabulce" << endl;
		throw invalid_argument("");
	}

	if (colNames.size() < this->windowWidth) {
		cout << "Větší šírka okna než počet sloupců v tabulce" << endl;
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


