#include <bits/stdc++.h>
using namespace std;
bool randomBool() {
    return rand() % 2 == 0;
}
int randomInt(int low, int high) {
    return low + rand() % (high - low + 1);
}
int main() {
    srand(time(0));
    int numVariables = randomInt(3, 7);
    int numClauses = randomInt(5, 10);
    int minLiteralsPerClause = 1;
    int maxLiteralsPerClause = 4;

    cout << "p cnf " << numVariables << " " << numClauses << endl;

    for (int i = 0; i < numClauses; ++i) {
        int clauseSize = randomInt(minLiteralsPerClause, maxLiteralsPerClause);
        set<int> usedVars;

        while (usedVars.size() < clauseSize) {
            int var = randomInt(1, numVariables);
            if (usedVars.count(var) == 0) {
                usedVars.insert(var);
                int lit = randomBool() ? var : -var;
                cout << lit << " ";
            }
        }

        cout << "0" << endl;
    }

    return 0;
}