#include <bits/stdc++.h>
using namespace std;

struct AssignmentEntry {
    bool value;
    int decisionLevel;
    int impliedBy;
};

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

// Apply unit propagation with implication tracking
bool unitPropagation(vector<set<int>>& formula,
                     map<int, AssignmentEntry>& assignment,
                     vector<int>& trail,
                     int decisionLevel,
                     int& conflictLiteral,
                     set<int>& conflictClause) {

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& clause : formula) {
            int unassigned = 0;
            int lastLit = 0;
            bool clauseSatisfied = false;

            for (int lit : clause) {
                int var = abs(lit);
                if (assignment.count(var)) {
                    bool val = assignment[var].value;
                    if ((lit > 0 && val) || (lit < 0 && !val)) {
                        clauseSatisfied = true;
                        break;
                    }
                } else {
                    unassigned++;
                    lastLit = lit;
                }
            }

            if (clauseSatisfied) continue;

            if (unassigned == 0) {
                // Conflict
                conflictClause = clause;
                conflictLiteral = 0;
                return false;
            }

            if (unassigned == 1) {
                int unit = lastLit;
                int var = abs(unit);
                bool val = unit > 0;

                assignment[var] = {val, decisionLevel, -unit};
                trail.push_back(unit);
                changed = true;

                // Remove satisfied clauses and simplify
                formula.erase(remove_if(formula.begin(), formula.end(), [&](const set<int>& c) {
                    return c.find(unit) != c.end();
                }), formula.end());

                for (auto& clause2 : formula) {
                    clause2.erase(-unit);
                }

                cout << "Unit propagated: " << unit << endl;
                break;
            }
        }
    }
    return true;
}

// Learn conflict clause (basic version: return clause as-is)
set<int> learnClause(const set<int>& conflictClause) {
    return conflictClause;
}

// Backtrack
void backtrack(map<int, AssignmentEntry>& assignment, vector<int>& trail, int backLevel) {
    cout << "Backtracking to level " << backLevel << endl;
    for (auto it = trail.rbegin(); it != trail.rend();) {
        int var = abs(*it);
        if (assignment[var].decisionLevel > backLevel) {
            assignment.erase(var);
            it = vector<int>::reverse_iterator(trail.erase(next(it).base()));
        } else {
            ++it;
        }
    }
}

// CDCL main function
bool cdcl(vector<set<int>> formula, map<int, AssignmentEntry>& assignment) {
    int decisionLevel = 0;
    vector<int> trail;

    while (true) {
        int conflictLiteral = 0;
        set<int> conflictClause;

        if (!unitPropagation(formula, assignment, trail, decisionLevel, conflictLiteral, conflictClause)) {
            // Conflict occurred
            if (decisionLevel == 0) {
                cout << "Conflict at level 0 => UNSAT\n";
                return false;
            }

            set<int> learned = learnClause(conflictClause);
            formula.push_back(learned);

            // Find highest decision level in conflict clause except one
            set<int> levels;
            for (int lit : learned) {
                int var = abs(lit);
                if (assignment.count(var)) {
                    levels.insert(assignment[var].decisionLevel);
                }
            }

            int backLevel = 0;
            if (levels.size() > 1) {
                auto it = levels.rbegin();
                ++it;
                backLevel = *it;
            }

            backtrack(assignment, trail, backLevel);
            decisionLevel = backLevel;
            continue;
        }

        // Check if satisfied
        bool allAssigned = true;
        for (const auto& clause : formula) {
            if (clause.empty()) {
                cout << "Conflict after simplification => UNSAT\n";
                return false;
            }
            bool satisfied = false;
            for (int lit : clause) {
                int var = abs(lit);
                if (assignment.count(var)) {
                    if ((lit > 0 && assignment[var].value) || (lit < 0 && !assignment[var].value)) {
                        satisfied = true;
                        break;
                    }
                }
            }
            if (!satisfied) {
                allAssigned = false;
                break;
            }
        }

        if (allAssigned) {
            cout << "SATISFIABLE\n";
            return true;
        }

        // Choose next decision variable
        int nextVar = 0;
        for (const auto& clause : formula) {
            for (int lit : clause) {
                int var = abs(lit);
                if (assignment.count(var) == 0) {
                    nextVar = var;
                    break;
                }
            }
            if (nextVar) break;
        }

        if (nextVar == 0) return false; // No variables left

        // Make decision
        decisionLevel++;
        bool val = true; // default branch
        assignment[nextVar] = {val, decisionLevel, 0};
        trail.push_back(val ? nextVar : -nextVar);
        cout << "Decision: " << nextVar << " = " << (val ? "true" : "false") << " at level " << decisionLevel << endl;
    }
}

// DIMACS parser
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

    map<int, AssignmentEntry> assignment;
    printFormula(formula);

    bool result = cdcl(formula, assignment);

    cout << "\nResult: " << (result ? "SATISFIABLE" : "UNSATISFIABLE") << endl;

    if (result) {
        cout << "Assignment:\n";
        for (const auto& [var, entry] : assignment) {
            cout << var << " = " << (entry.value ? "true" : "false") << endl;
        }
    }

    return 0;
}
