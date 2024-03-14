#include <iostream>

#include "HelperFunc.h"

using namespace std;

bool HelperFunc::textContainsOnlySmallLetters(const string & text)
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
