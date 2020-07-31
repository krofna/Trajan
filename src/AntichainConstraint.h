    /*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef ANTICHAIN_CONSTRAINT_H
#define ANTICHAIN_CONSTRAINT_H

#include "Constraint.h"
#include <mutex>
#include <queue>

class AntichainConstraint : Constraint
{
public:
    AntichainConstraint(MPSolver& solver, vector<ET>& Triplets, vector<MPConstraint*>& constraints, vector<MPVariable*>& variables, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp);
    int AddTriplets(int nr_rows);

private:
    void RunParallel();
    void AntichainJob(int id);
    double MaxFlow(vi& D, vector<vd>& R);
    double Push(int x, double flow, vector<vd>& R, vi& D);
    bool LevelGraph(vi& D, vector<vd>& R);
    double Reset(vi& P, vector<vd>& R);
    void Antichain(vi& P, vector<vd>& R);

    vector<vi> &G;
    int ncr, nr_rows, S, T, Z, SZ;
    mutex qmutex;
    vb B;
    int pi;
};

#endif
