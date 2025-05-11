#include <bits/stdc++.h>
using namespace std;

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

bool applyUnitPropagation(vector<set<int>>& formula, map<int, bool>& assignment) {
    for (const auto& clause : formula) {
        if (clause.size() == 1) {
            int unit = *clause.begin();
            int var = abs(unit);
            bool val = unit > 0;
            assignment[var] = val;

            formula.erase(remove_if(formula.begin(), formula.end(), [&](const set<int>& c) {
                return c.find(unit) != c.end();
            }), formula.end());

            for (auto& clause2 : formula) {
                clause2.erase(-unit);
            }

            cout << "Applied unit propagation with literal: " << unit << endl;
            return true;
        }
    }
    return false;
}

bool applyPureLiteral(vector<set<int>>& formula, map<int, bool>& assignment) {
    map<int, int> counts;
    for (const auto& clause : formula) {
        for (int lit : clause) {
            counts[lit]++;
        }
    }

    for (const auto& [lit, _] : counts) {
        int var = abs(lit);
        if (assignment.find(var) != assignment.end()) continue;

        if (counts.find(-lit) == counts.end()) {
            assignment[var] = lit > 0;
            formula.erase(remove_if(formula.begin(), formula.end(), [&](const set<int>& c) {
                return c.find(lit) != c.end();
            }), formula.end());

            cout << "Applied pure literal elimination with literal: " << lit << endl;
            return true;
        }
    }
    return false;
}

int chooseVariable(const vector<set<int>>& formula, const map<int, bool>& assignment) {
    for (const auto& clause : formula) {
        for (int lit : clause) {
            int var = abs(lit);
            if (assignment.find(var) == assignment.end())
                return var;
        }
    }
    return 0;
}

bool dpll(vector<set<int>> formula, map<int, bool>& assignment) {
    while (true) {
        if (applyUnitPropagation(formula, assignment)) {
            printFormula(formula);
            continue;
        }

        if (applyPureLiteral(formula, assignment)) {
            printFormula(formula);
            continue;
        }

        for (const auto& clause : formula)
            if (clause.empty()) {
                cout << "Derived empty clause => UNSATISFIABLE\n";
                return false;
            }

        if (formula.empty()) {
            cout << "Clause set empty => SATISFIABLE\n";
            return true;
        }

        break;
    }

    int var = chooseVariable(formula, assignment);
    if (var == 0) return false;

    for (bool val : {true, false}) {
        map<int, bool> newAssign = assignment;
        newAssign[var] = val;
        vector<set<int>> newFormula;

        for (const auto& clause : formula) {
            if (clause.find(val ? var : -var) != clause.end())
                continue;

            set<int> newClause = clause;
            newClause.erase(val ? -var : var);
            newFormula.push_back(newClause);
        }
        cout << "Branching on variable " << var << " = " << (val ? "true" : "false") << endl;
        printFormula(newFormula);
        if (dpll(newFormula, newAssign)) {
            assignment = newAssign;
            return true;
        }
    }

    return false;
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
        cerr << "Error: could not open 'example.txt'\n";
        return 1;
    }

    vector<set<int>> formula = parseDIMACS(fin);
    fin.close();

    map<int, bool> assignment;
    printFormula(formula);
    bool result = dpll(formula, assignment);

    cout << "\nResult: " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << endl;

    if (result) {
        cout << "Assignment:\n";
        for (const auto& [var, val] : assignment) {
            cout << var << " = " << (val ? "true" : "false") << endl;
        }
    }

    return 0;
}
