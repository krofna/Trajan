/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#include "Geno.h"
#include "Graph.h"
#include <utility>
using namespace std;

#include <ortools/linear_solver/linear_solver.h>
using namespace operations_research;

const double EPS = 1e-2;

typedef vector<pair<int, int>> vii;

class Constraint
{
public:
    Constraint(MPSolver& solver, vector<ET>& Triplets, vector<MPConstraint*>& constraints, vector<MPVariable*>& variables, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp);

    virtual int AddTriplets(int nr_rows) = 0;
protected:
    inline int GetCol(int i, int j) const;
    inline double GetWeight(int nodel, int noder) const;
    void AddConstraint(int row, vii& P);

    MPSolver& solver;
    vector<MPConstraint*>& constraints;
    vector<MPVariable*>& variables;
    vector<ET> &Triplets;
    Graph &t1, &t2;
    vector<vi> &K;
    Vector &x;
    bool swp;
};

inline int Constraint::GetCol(int i, int j) const
{
    return swp ? K[j][i] : K[i][j];
}

inline double Constraint::GetWeight(int i, int j) const
{
    int in = GetCol(i, j);
    return in == -1 ? 0 : x(in);
}

#endif
