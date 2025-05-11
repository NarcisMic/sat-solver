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

bool applyUnitPropagation(vector<set<int>>& formula) {
    for (const auto& clause : formula) {
        if (clause.size() == 1) {
            int unit = *clause.begin();
            formula.erase(remove_if(formula.begin(), formula.end(), [&](const set<int>& c) {
                return c.find(unit) != c.end();
            }), formula.end());

            int comp = -unit;
            for (auto& clause2 : formula) {
                clause2.erase(comp);
            }

            return true;
        }
    }
    return false;
}

bool applyPureLiteral(vector<set<int>>& formula) {
    map<int, int> literalCount;
    for (const auto& clause : formula) {
        for (int lit : clause) {
            literalCount[lit]++;
        }
    }

    for (const auto& [lit, _] : literalCount) {
        int negLit = -lit;
        if (literalCount.find(negLit) == literalCount.end()) {
            formula.erase(remove_if(formula.begin(), formula.end(), [&](const set<int>& c) {
                return c.find(lit) != c.end();
            }), formula.end());
            return true;
        }
    }
    return false;
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

bool davisPutnam(vector<set<int>> formula) {
    cout << "Initial formula:\n";
    printFormula(formula);

    while (true) {
        if (applyUnitPropagation(formula)) {
            cout << "\nApplied unit propagation:\n";
            printFormula(formula);
            continue;
        }

        if (applyPureLiteral(formula)) {
            cout << "\nApplied pure literal rule:\n";
            printFormula(formula);
            continue;
        }

        for (const auto& clause : formula) {
            if (clause.empty()) {
                cout << "Empty clause derived => UNSATISFIABLE\n";
                return false;
            }
        }

        if (formula.empty()) {
            cout << "Clause set empty => SATISFIABLE\n";
            return true;
        }

        bool addedNew = false;
        vector<set<int>> newClauses;

        for (size_t i = 0; i < formula.size(); ++i) {
            for (size_t j = i + 1; j < formula.size(); ++j) {
                vector<set<int>> resolvents = resolveClauses(formula[i], formula[j]);
                for (const auto& res : resolvents) {
                    if (res.empty()) {
                        cout << "Derived empty clause from ";
                        printClause(formula[i]);
                        cout << " and ";
                        printClause(formula[j]);
                        cout << " => UNSATISFIABLE\n";
                        return false;
                    }
                    if (!clauseExists(formula, res)) {
                        newClauses.push_back(res);
                        addedNew = true;
                        cout << "Adding resolvent ";
                        printClause(res);
                        cout << endl;
                    }
                }
            }
        }

        if (!addedNew) break;
        formula.insert(formula.end(), newClauses.begin(), newClauses.end());
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

    bool result = davisPutnam(formula);
    return 0;
}