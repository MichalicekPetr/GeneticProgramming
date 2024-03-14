// GeneticPrograming.cpp : Tento soubor obsahuje funkci main. Provádění programu se tam zahajuje a ukončuje.
//

#include <iostream>
#include <memory>

#include "Connection.h"
#include "Crossover.h"
#include "Fitness.h"
#include "Function.h"
#include "GeneticProgramming.h"
#include "Individual.h"
#include "Mutation.h"
#include "Random.h"
#include "Terminal.h"
#include "Test.h"

using namespace std;

int main()
{


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
        
        cout << "Press 1 for genetic programming or 2 for genetic programming with constant tuning" << endl;
        int choice;
        cin >> choice;

        if (choice == 1) {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testdb", 3306);
            vector<string> colNames = connection.getColNames("testdb", "table_c");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();

            int popSize = 200;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            double subtreeMutProb = 0.05;
            double replaceNodeMutProb = 0.01;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.58;
            double leafPickProb = 0.1;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testdb";
            string tableName = "table_c";
            string primaryKey = "idx";
            bool saveDbToMemory = true;
            geneticProgramming.setDbThings(shared_ptr<Connection>(new MysqlConnection()), dbName, tableName, primaryKey, saveDbToMemory);

            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);

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

            geneticProgramming.standartRun(100, 4);
        }
        else {
            MysqlConnection connection;
            connection.connectToDb("localhost", "root", "krtek", "testdb", 3306);
            vector<string> colNames = connection.getColNames("testdb", "table_c");
            colNames.erase(std::remove(colNames.begin(), colNames.end(), "y"), colNames.end());

            FunctionSet funcSet = FunctionSet::createArithmeticFunctionSet();
            TerminalSet termSet = TerminalSet(-5, 5, false, colNames);

            GeneticProgramming geneticProgramming = GeneticProgramming();

            int popSize = 40;
            geneticProgramming.setPopulation(Population(popSize, unique_ptr<PopulationInitMethod>(new RandomHalfFullHalfGrowInitialization())));

            geneticProgramming.setFunctionSet(funcSet);
            geneticProgramming.setTerminalSet(termSet);

            double subtreeMutProb = 0.05;
            double replaceNodeMutProb = 0.01;
            geneticProgramming.setMutation(unique_ptr<Mutation>(new CombinedMutation(subtreeMutProb, replaceNodeMutProb, funcSet, termSet)));

            int tournamentSize = 4;
            geneticProgramming.setSelection(unique_ptr<Selection>(new TournamentSelection(tournamentSize)));

            double crossoverProb = 0.58;
            double leafPickProb = 0.1;
            geneticProgramming.setCrossover(unique_ptr<Crossover>(new TwoPointCrossover(leafPickProb)), crossoverProb);

            geneticProgramming.setFitness(unique_ptr<FitnessFunction>(new ClassicFitnessFunction()));

            string dbName = "testdb";
            string tableName = "table_c";
            string primaryKey = "idx";
            bool saveDbToMemory = true;
            geneticProgramming.setDbThings(shared_ptr<Connection>(new MysqlConnection()), dbName, tableName, primaryKey, saveDbToMemory);

            string target = "y";
            geneticProgramming.setTarget(target);

            string url = "localhost";
            string user = "root";
            string password = "krtek";
            int port = 3306;
            geneticProgramming.setLoginParams(url, user, password, port);

            double randomIndividualProb = 0.02;
            geneticProgramming.setRandomIndividualProb(randomIndividualProb);

            bool constantTuning = true;
            double constantTuningMaxTime = 0.2;
            geneticProgramming.setTuneConstants(constantTuning, constantTuningMaxTime);

            double vectorGA_crossoverProb = 0.5;
            double vectorGA_mutationProb = 0.01;
            int vectorGA_populationSize = 60;
            int vectorGA_tournamentSize = 4;
            double vectorGA_randomIndividualProb = 0.02;
            double vectorGA_newIndividualRatio = 0.8;
            geneticProgramming.setVectorGAParams(vectorGA_crossoverProb, vectorGA_mutationProb, vectorGA_tournamentSize,
                vectorGA_randomIndividualProb, vectorGA_populationSize, vectorGA_newIndividualRatio);

            geneticProgramming.standartRun(100, 4);
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
}
