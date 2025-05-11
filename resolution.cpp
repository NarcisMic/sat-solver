#include <bits/stdc++.h>

using namespace std;

bool isComplement(int a, int b) {
    return a == -b;
}

void printClause(const set<int>& clause) {
    cout << "(";
    bool first = true;
    for (int lit : clause) {
        if (!first) cout << " v ";
        cout << lit;
        first = false;
    }
    cout << ")";
}

void printFormula(const vector<set<int>>& formula) {
    cout << "Clause set:" << endl;
    for (const auto& clause : formula) {
        printClause(clause);
        cout << endl;
    }
}

vector<set<int>> resolveClauses(const set<int>& c1, const set<int>& c2) {
    vector<set<int>> resolvents;
    for (int lit1 : c1) {
        for (int lit2 : c2) {
            if (isComplement(lit1, lit2)) {
                set<int> resolvent;
                for (int l : c1) if (l != lit1) resolvent.insert(l);
                for (int l : c2) if (l != lit2) resolvent.insert(l);
                resolvents.push_back(resolvent);
            }
        }
    }
    return resolvents;
}

bool clauseExists(const vector<set<int>>& clauses, const set<int>& newClause) {
    return any_of(clauses.begin(), clauses.end(), [&](const set<int>& clause) {
        return clause == newClause;
    });
}

bool resolution(vector<set<int>> clauses) {
    cout << "Initial clause set:\n";
    printFormula(clauses);
    cout << endl;

    vector<set<int>> Kprime = clauses;
    bool addedNewClause = true;

    while (addedNewClause) {
        addedNewClause = false;
        vector<set<int>> newClauses;

        for (size_t i = 0; i < Kprime.size(); ++i) {
            for (size_t j = i + 1; j < Kprime.size(); ++j) {
                auto resolvents = resolveClauses(Kprime[i], Kprime[j]);
                for (const auto& res : resolvents) {
                    if (res.empty()) {
                        cout << "Derived empty clause from ";
                        printClause(Kprime[i]);
                        cout << " and ";
                        printClause(Kprime[j]);
                        cout << " => UNSATISFIABLE\n";
                        return false;
                    }
                    if (!clauseExists(Kprime, res)) {
                        cout << "Adding resolvent ";
                        printClause(res);
                        cout << " from ";
                        printClause(Kprime[i]);
                        cout << " and ";
                        printClause(Kprime[j]);
                        cout << endl;
                        newClauses.push_back(res);
                    }
                }
            }
        }

        if (!newClauses.empty()) {
            Kprime.insert(Kprime.end(), newClauses.begin(), newClauses.end());
            addedNewClause = true;
        }
    }

    cout << "\nNo new clauses can be derived. Formula is SATISFIABLE.\n";
    return true;
}

vector<set<int>> parseDIMACS(ifstream& in) {
    vector<set<int>> formula;
    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == 'c') continue;
        if (line[0] == 'p') continue;                

        stringstream ss(line);
        int lit;
        set<int> clause;

        while (ss >> lit) {
            if (lit == 0) break;
            clause.insert(lit);
        }

        if (!clause.empty())
            formula.push_back(clause);
    }
    return formula;
}

int main() {
    ifstream fin("example.txt");
    if (!fin) {
        cerr << "Error: could not open 'example.txt'. Please ensure the file exists.\n";
        return 1;
    }

    auto formula = parseDIMACS(fin);
    fin.close();

    bool result = resolution(formula);
    return 0;
}
