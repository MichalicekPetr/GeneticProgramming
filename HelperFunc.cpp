#include <iostream>
#include <vector>
#include <cmath>

#include "HelperFunc.h"

using namespace std;

bool HelperFunc::textContainsOnlySmallLetters(const string& text)
{
    for (char ch : text) {
        if (!islower(ch)) {
            return false;
        }
    }
    return true;
}

void HelperFunc::printDashLine()
{
    cout << "----------------------------------------------------------------------------------------------------------" << endl;
}

vector<double> HelperFunc::generateNormalSizeDistribution(int maxSize, double mean, double stddev)
{
    if (mean < 0) mean = maxSize / 2.0;      // implicitní støed uprostøed
    if (stddev < 0) stddev = maxSize / 6.0;  // implicitní rozptyl (vìtšina hodnot v rozsahu)

    std::vector<double> distribution(maxSize);
    double sum = 0.0;

    for (int i = 0; i < maxSize; ++i) {
        // Gaussova køivka: f(x) = exp(-0.5 * ((x - mean)/stddev)^2)
        double x = i + 1; // velikosti od 1 do maxSize
        double prob = std::exp(-0.5 * std::pow((x - mean) / stddev, 2));
        distribution[i] = prob;
        sum += prob;
    }

    // Normalizace na souèet 1
    for (int i = 0; i < maxSize; ++i) {
        distribution[i] /= sum;
    }

    return distribution;
}