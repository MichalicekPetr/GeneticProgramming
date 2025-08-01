﻿// GeneticPrograming.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <fstream>
#include <chrono>
#include <iostream>
#include <memory>
#include <map>

#include "Connection.h"
#include "Crossover.h"
#include "DatabaseScripts.h"
#include "Fitness.h"
#include "Function.h"
#include "HelperFunc.h"
#include "GeneticProgramming.h"
#include "Individual.h"
#include "Mutation.h"
#include "Random.h"
#include "Terminal.h"
#include "Test.h"

using namespace std;

void tuneHyperParamatersGP() {
    vector<double> subtreeMutationProbs({0.01, 0.03, 0.1});
    vector<double> nodeMutationProbs({0.001, 0.01, 0.03});
    vector<int> tournamentSizes({3, 5, 7});
    vector<double> crossoverProbs({0.3, 0.5, 0.7});
    vector<double> randomIndividualProbs({0.01, 0.03, 0.1});

    ofstream file("C:/Users/petrm/Desktop/GeneticPrograming/Data/Hyperparamtuning.txt");

    MysqlConnection connection;
    connection.connectToDb("localhost", "root", "krtek", "sys", 3306);
    vector<string> colNames = connection.getColNames("testdb", "table_c");
    colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

    for (const auto x1 : subtreeMutationProbs) {
        for (const auto x2 : nodeMutationProbs) {
            for (const auto x3 : tournamentSizes) {
                for (const auto x4 : crossoverProbs) {
                    for (const auto x5 : randomIndividualProbs) {
                        cout << x1 << " " << x2 << " " << x3 << " " << x4 << " " << x5 << endl;
                        FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
                        TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

                        GeneticProgramming geneticProgramming = GeneticProgramming();

                        
                        int threadCnt = 3;
                        geneticProgramming.setThreadCnt(threadCnt);

                        int popSize = 200;
                        geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

                        geneticProgramming.setFunctionSet(funcSet);
                        geneticProgramming.setTerminalSet(termSet);

                        double subtreeMutProb = x1;
                        double replaceNodeMutProb = x2;
                        geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

                        int tournamentSize = x3;
                        geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

                        double crossoverProb = x4;
                        double leafPickProb = 0.1;
                        double subtreeLeafPickProb = 0.1;
                        double parentLeafPickProb = 0.8;
                        geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

                        geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

                        string dbName = "testdb";
                        string tableName = "table_c";
                        string primaryKey = "idx";
                        bool saveDbToMemory = true;

                        string target = "y";
                        geneticProgramming.setTarget(target);

                        string url = "localhost";
                        string user = "root";
                        string password = "krtek";
                        int port = 3306;
                        geneticProgramming.setLoginParams(url, user, password, port);
                        geneticProgramming.initConnections(
                            shared_ptr<Connection>(new MysqlConnection()),
                            threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                            dbName, tableName, primaryKey, saveDbToMemory
                        );

                        double randomIndividualProb = x5;
                        geneticProgramming.setRandomIndividualProb(randomIndividualProb);

                        bool constantTuning = false;
                        double constantTuningMaxTime = 1.0;
                        geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

                        double vectorGA_crossoverProb = 0.5;
                        double vectorGA_mutationProb = 0.01;
                        int vectorGA_populationSize = 50;
                        int vectorGA_tournamentSize = 4;
                        double vectorGA_randomIndividualProb = 0.02;
                        double vectorGA_newIndividualRatio = 0.8;
                        geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                            vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

                        bool datFile = true;
                        string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
                        string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
                        geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

                        bool useWindow = false;
                        int windowHeight = 10;
                        int windowWidth = 2;
                        geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);

                        double elapsedTime;
                        double acc = 0;
                        for (int i = 0; i < 20; i++) {
                            chrono::steady_clock::time_point beginTime = chrono::steady_clock::now();
                            geneticProgramming.standartRun(1000, 4, false);
                            chrono::steady_clock::time_point currentTime = chrono::steady_clock::now();   
                            elapsedTime = chrono::duration_cast<std::chrono::milliseconds> (currentTime - beginTime).count();
                            acc += (elapsedTime / 1000);
                        }
                        file << x1 << " ; " << x2 << " ; " << x3 << " ; " << x4 << " ; " << x5 << " ---------> " << acc / 25  << endl;
                    }
                }
            }
        }
    }
    file.close();
}

void tuneHyperParametersGAGP(){

}

int main()
{
    mysql_library_init(0, nullptr, nullptr);

    try {
        cout << "Starting program\n" << endl;

        /*

        unique_ptr<Connection> connPtr(new MysqlConnection());
        connPtr->connectToDb("localhost", "root", "krtek", "testdb", 3306);
        vector<string> colNames = connPtr->getColNames("testdb", "table_c");
        colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

        FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
        TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

        Individual tree = Individual::generateRandomTreeFullMethod(3, funcSet, termSet);
        

        cout << tree << endl;

        tree.createConstantTable();

        cout << "Result: " << tree.evaluateTree(connPtr, "testdb", "table_c", 1) << endl;

        cout << tree << endl;

        vector<double> arr{ 2.0, 4.0, 6.0, 8.0 };

        tree.getConstantTableRef().setTable(arr);

        cout << "Result: " << tree.evaluateTree(connPtr, "testdb", "table_c", 1) << endl;

        cout << tree << endl;

       
        //Individual tree = Individual::generateRandomTreeGrowMethod(4, funcSet, termSet);
        cout << tree << endl;
        
        ClassicFitnessFunction fitnessFunc = ClassicFitnessFunction();
        double result = fitnessFunc.evaluate(tree, connection, "testdb", "table_a", "y", "idx");

        cout << "Result: " << result << endl;
       
        FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
        TerminalSet termSet = TerminalSet(-5, 5, false, colNames);
        Individual tree1 = Individual::generateRandomTreeFullMethod(4, funcSet, termSet);
        Individual tree2 = Individual::generateRandomTreeFullMethod(4, funcSet, termSet);
        
        cout << "Tree before mutation:" << endl << tree1 << endl;

        TwoPointCrossover crossover = TwoPointCrossover(0.5);
        Individual offspring = crossover.createOffspring(tree1, tree2);

        SubtreeMutation mut = SubtreeMutation(1.0, funcSet, termSet);
        mut.mutate(tree1);
        cout << "Tree after mutation:" << endl << tree1 << endl;
         */
        
        cout << "Press key and choose mode:\n"
            "1 for genetic programming\n"
            "2 for genetic programming with constant tuning\n"
            "3 for genetic programming with window\n"
            "4 for show table create script\n"
            "5 for show row insert script\n"
            "6 for hyperparam tuning\n"
            "7 for array representation test\n"
            "8 Markov chains experiment\n"
            "9 for PCT2 algorithm test\n"
            "A pro experiment na měření rychlosti generování stromů\n" << endl;
        int choice;
        cin >> choice;

        if (choice == 1) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
            vector<string> colNames = connection.getColNames("testschema", "testdb1");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());
            for(auto x : colNames){
                cout << x << endl;
            }

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();

            int popSize = 200;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            int threadCnt = 3;
            geneticProgramming.setThreadCnt(threadCnt);

            double subtreeMutProb = 0.05;
            double replaceNodeMutProb = 0.01;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.7;
            double leafPickProb = 0.1;
            double subtreeLeafPickProb = 0.1;
            double parentLeafPickProb = 0.8;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testschema";
            string tableName = "testdb1";
            string primaryKey = "idx";
            bool saveDbToMemory = true;

            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);
            geneticProgramming.initConnections(
                shared_ptr<Connection>(new MysqlConnection()),
                threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                dbName, tableName, primaryKey, saveDbToMemory
            );

            double randomIndividualProb = 0.02;
            geneticProgramming.setRandomIndividualProb(randomIndividualProb);

            bool constantTuning = false;
            double constantTuningMaxTime = 1.0;
            geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

            double vectorGA_crossoverProb = 0.5;
            double vectorGA_mutationProb = 0.01;
            int vectorGA_populationSize = 50;
            int vectorGA_tournamentSize = 4;
            double vectorGA_randomIndividualProb = 0.02;
            double vectorGA_newIndividualRatio = 0.8;
            geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

            bool datFile = true;
            string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
            string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
            geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

            bool useWindow = false;
            int windowHeight = 10;
            int windowWidth = 2;
            geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);

            geneticProgramming.standartRun(1000, 4);
        }
        else if (choice == 2) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
            vector<string> colNames = connection.getColNames("testschema", "testdb1");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();

            int popSize = 100;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));


            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            int threadCnt = 3;
            geneticProgramming.setThreadCnt(threadCnt);

            double subtreeMutProb = 0.06;
            double replaceNodeMutProb = 0.03;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.7;
            double leafPickProb = 0.1;
            double subtreeLeafPickProb = 0.1;
            double parentLeafPickProb = 0.8;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testschema";
            string tableName = "testdb1";
            string primaryKey = "idx";
            bool saveDbToMemory = true;

            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);
            geneticProgramming.initConnections(
                shared_ptr<Connection>(new MysqlConnection()),
                threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                dbName, tableName, primaryKey, saveDbToMemory
            );

            double randomIndividualProb = 0.04;
            geneticProgramming.setRandomIndividualProb(randomIndividualProb);

            bool constantTuning = true;
            double constantTuningMaxTime = 2;
            geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

            double vectorGA_crossoverProb = 0.7;
            double vectorGA_mutationProb = 0.03;
            int vectorGA_populationSize = 50;
            int vectorGA_tournamentSize = 4;
            double vectorGA_randomIndividualProb = 0.03;
            double vectorGA_newIndividualRatio = 0.8;
            geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

            bool datFile = true;
            string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
            string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
            geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

            bool useWindow = false;
            int windowHeight = 2;
            int windowWidth = 10;
            geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);

            geneticProgramming.setMaxDepth(5);

            geneticProgramming.standartRun(100, 4);
        }
        else if(choice ==3){
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testdb", 3306);
            vector<string> colNames = connection.getColNames("testdb", "table_c");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();

            int popSize = 200;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

            int threadCnt = 3;
            geneticProgramming.setThreadCnt(threadCnt);

            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            double subtreeMutProb = 0.05;
            double replaceNodeMutProb = 0.01;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.7;
            double leafPickProb = 0.1;
            double subtreeLeafPickProb = 0.1;
            double parentLeafPickProb = 0.8;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testdb";
            string tableName = "table_c";
            string primaryKey = "idx";
            bool saveDbToMemory = true;

            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);
            geneticProgramming.initConnections(
                shared_ptr<Connection>(new MysqlConnection()),
                threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                dbName, tableName, primaryKey, saveDbToMemory
            );

            double randomIndividualProb = 0.02;
            geneticProgramming.setRandomIndividualProb(randomIndividualProb);

            bool constantTuning = false;
            double constantTuningMaxTime = 1.0;
            geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

            double vectorGA_crossoverProb = 0.5;
            double vectorGA_mutationProb = 0.01;
            int vectorGA_populationSize = 50;
            int vectorGA_tournamentSize = 4;
            double vectorGA_randomIndividualProb = 0.02;
            double vectorGA_newIndividualRatio = 0.8;
            geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

            bool datFile = true;
            string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
            string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
            geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

            bool useWindow = true;
            int windowHeight = 10;
            int windowWidth = 2;
            geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);
            geneticProgramming.standartRun(1000, 4);
        }
        else if (choice == 4) {
            DatabaseScripts::createTableInsertScriptTestDb1();
        }
        else if (choice == 5) {
            DatabaseScripts::createRowInsertScriptTestDb1();
        }
        else if (choice == 6) {
            tuneHyperParamatersGP();
        }
        else if (choice == 7) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
            vector<string> colNames = connection.getColNames("testschema", "testdb1");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();
			int threadCnt = 7;
            geneticProgramming.setThreadCnt(threadCnt);

            int popSize = 50;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            double subtreeMutProb = 0.05;
            double replaceNodeMutProb = 0.01;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(replaceNodeMutProb, subtreeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.7;
            double leafPickProb = 0.1;
			double subtreeLeafPickProb = 0.1;
			double parentLeafPickProb = 0.8;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testschema";
            string tableName = "testdb1";
            string primaryKey = "idx";
            bool saveDbToMemory = true;


            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);

            geneticProgramming.initConnections(
                shared_ptr<Connection>(new MysqlConnection()),
                threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                dbName, tableName, primaryKey, saveDbToMemory
            );

            double randomIndividualProb = 0.04;
            geneticProgramming.setRandomIndividualProb(randomIndividualProb);

            bool constantTuning = true;
            double constantTuningMaxTime = 1;
            geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

            double vectorGA_crossoverProb = 0.7;
            double vectorGA_mutationProb = 0.03;
            int vectorGA_populationSize = 50;
            int vectorGA_tournamentSize = 4;
            double vectorGA_randomIndividualProb = 0.03;
            double vectorGA_newIndividualRatio = 0.8;
            geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

            bool datFile = false;
            string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
            string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
            geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

            bool useWindow = false;
            int windowHeight = 2;
            int windowWidth = 10;
            geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);

            geneticProgramming.setMaxDepth(10);

			bool mergeConstantOptimalization = false;
			bool removeUselessBranchesOptimalization = false;
			bool DAGOptimalization = false;
			geneticProgramming.setOptimalizationParams(mergeConstantOptimalization, removeUselessBranchesOptimalization, DAGOptimalization);

            geneticProgramming.standartRun(1000, 3, false);
        }
        else if (choice == 8) {
            for (int d = 6; d <= 10; d++) {
                for (int i = 0; i < 10; i++) {
                    
                    MysqlConnection connection;
                    connection.connectToDb("localhost", "root", "krtek", "markov", 3306);
                    vector<string> colNames = connection.getColNames("markov", "table_markov1");
                    colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

                    FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
                    TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

                    GeneticProgramming geneticProgramming = GeneticProgramming();
                    int threadCnt =15;
                    geneticProgramming.setThreadCnt(threadCnt);

                    int popSize = 100;
                    geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

                    geneticProgramming.setFunctionSet(funcSet);
                    geneticProgramming.setTerminalSet(termSet);

                    double subtreeMutProb = 0.09;
                    double replaceNodeMutProb = 0.0175;
                    geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(replaceNodeMutProb, subtreeMutProb, funcSet, termSet)));

                    int tournamentSize = 3;
                    geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

                    double crossoverProb = 0.7;
                    double leafPickProb = 0.1;
                    double subtreeLeafPickProb = 0.1;
                    double parentLeafPickProb = 0.8;
                    geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb, subtreeLeafPickProb, parentLeafPickProb)), crossoverProb);

                    geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

                    string dbName = "markov";
                    string tableName = "table_markov1";
                    string primaryKey = "idx";
                    bool saveDbToMemory = true;


                    string target = "y";
                    geneticProgramming.setTarget(target);

                    string url = "localhost";
                    string user = "root";
                    string password = "krtek";
                    int port = 3306;
                    geneticProgramming.setLoginParams(url, user, password, port);

                    geneticProgramming.initConnections(
                        shared_ptr<Connection>(new MysqlConnection()),
                        threadCnt, // nebo zadejte konkrétní počet vláken, např. 1
                        dbName, tableName, primaryKey, saveDbToMemory
                    );

                    double randomIndividualProb = 0.1;
                    geneticProgramming.setRandomIndividualProb(randomIndividualProb);

                    bool constantTuning = true;
                    double constantTuningMaxTime = 2;
                    geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

                    double vectorGA_crossoverProb = 0.7;
                    double vectorGA_mutationProb = 0.03;
                    int vectorGA_populationSize = 50;
                    int vectorGA_tournamentSize = 4;
                    double vectorGA_randomIndividualProb = 0.03;
                    double vectorGA_newIndividualRatio = 0.8;
                    geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                        vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

                    bool datFile = false;
                    string GPdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP/";
                    string GPGAdataFolderPath = "C:/Users/petrm/Desktop/GeneticPrograming/Data/DataFiles/GP+GA/";
                    geneticProgramming.setOutputFileParams(datFile, GPdataFolderPath, GPGAdataFolderPath);

                    bool useWindow = false;
                    int windowHeight = 2;
                    int windowWidth = 10;
                    geneticProgramming.setWindowParams(useWindow, windowHeight, windowWidth);

                    geneticProgramming.setMaxDepth(d);

                    bool mergeConstantOptimalization = false;
                    bool removeUselessBranchesOptimalization = false;
                    bool DAGOptimalization = false;
                    geneticProgramming.setOptimalizationParams(mergeConstantOptimalization, removeUselessBranchesOptimalization, DAGOptimalization);

                    ofstream file("C:/Users/petrm/Desktop/Markov/Results/markov_results.txt", std::ios::app);
					file << "====================================================================" << endl;
                    file << "Depth: " << d << ", Iteration: " << i << endl;
                    file.close();

                    geneticProgramming.standartRun(1000, d, false);

                }
            }   
        }
        /*
        else if (choice == 9) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
            vector<string> colNames = connection.getColNames("testschema", "testdb1");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());
            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            map<string, double> pmap = {
                {"+", 0.3}, {"*", 0.3}, {"-", 0.1}, {"%", 0.05}, {"neg", 0.2}, {"inv", 0.05}
            };
            Individual test;
            vector<double> sizeDistribution = HelperFunc::generateNormalSizeDistribution(15);
            for (int i = 0; i < 10; i++) {
                cout << "Iteration: " << i << endl;
                test = Individual::generateRandomTreePCT2(4, sizeDistribution, funcSet, termSet, pmap);
                cout << test << endl;
            }
        }*/
        else if (choice == 9) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
            vector<string> colNames = connection.getColNames("testschema", "testdb1");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());
            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);
            map<string, double> pmap = {
            {"+", 0.3}, {"*", 0.3}, {"-", 0.1}, {"%", 0.05}, {"neg", 0.2}, {"inv", 0.05} };


			int treeCnt = 100;
            int d = 15;

			cout << "Size of poulation: " << treeCnt << endl;
			cout << "Depth of individual (tree): " << d << endl;

            vector<double> sizeDistribution = HelperFunc::generateNormalSizeDistribution(pow(2,d-1));

            auto start1 = std::chrono::high_resolution_clock::now();
			vector<Individual> population1;
            for (int i = 0; i < treeCnt; i++) {
                Individual tree = Individual::generateRandomTreeFullMethod(d, funcSet, termSet);
				population1.push_back(tree);
			}
            auto end1 = std::chrono::high_resolution_clock::now();
            auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count();
            cout << "Creating population took " << duration1 << " ms with full method" << endl;
           
            auto start2 = std::chrono::high_resolution_clock::now();
            vector<Individual> population2;
            for (int i = 0; i < treeCnt; i++) {
                Individual tree = Individual::generateRandomTreeGrowMethod(d, funcSet, termSet);
                population2.push_back(tree);
            }
            auto end2 = std::chrono::high_resolution_clock::now();
            auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2).count();
            cout << "Creating population took " << duration2 << " ms with grow method" << endl;

            auto start3 = std::chrono::high_resolution_clock::now();
            vector<Individual> population3;
            for (int i = 0; i < treeCnt; i++) {
                Individual tree = Individual::generateRandomTreePCT1(d, pow(2, d-2), funcSet, termSet, pmap);
                population3.push_back(tree);
            }
            auto end3 = std::chrono::high_resolution_clock::now();
            auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(end3 - start3).count();
            cout << "Creating population took " << duration3 << " ms with PCT1" << endl;
            
            auto start4 = std::chrono::high_resolution_clock::now();
            vector<Individual> population4;
            for (int i = 0; i < treeCnt; i++) {
                Individual tree = Individual::generateRandomTreePCT2(d, sizeDistribution, funcSet, termSet, pmap);
                population4.push_back(tree);
            }
            auto end4 = std::chrono::high_resolution_clock::now();
            auto duration4 = std::chrono::duration_cast<std::chrono::milliseconds>(end4 - start4).count();
            cout << "Creating population took " << duration4 << " ms with PCT2" << endl;


		}
        else {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testschema", 3306);
        }


        cout << "Ending program\n" << endl;
        while (true) {
            string s;
            cin >> s;
            if (s == "end")
                break;
        }

        
    }
    catch (exception e) {
        cout << "Exception: " << e.what() << endl;
    }
    mysql_library_end();
}
