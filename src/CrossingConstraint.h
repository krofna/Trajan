/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef CROSSING_CONSTRAINT_H
#define CROSSING_CONSTRAINT_H

#include "Constraint.h"
#include <mutex>
#include <queue>
#include <atomic>
#include <cstdint>

class CrossingConstraint : Constraint
{
public:
    CrossingConstraint(MPSolver& solver, vector<ET>& Triplets, vector<MPConstraint*>& constraints, vector<MPVariable*>& variables, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp);
    
    int AddTriplets(int nr_rows);
private:
    pair<int, double> GetMaxPC(int nodel, const vector<int>& noder, bool s);
    inline pair<int, double> GetMaxChild(int nodel, int noder);
    inline pair<int, double> GetMaxParent(int nodel, int noder);

    void RunParallel();
    void CrossingJob(int i);
    double DFSRight(int node, int nodel);
    void Reconstruct(vii& P, int nodel, int noder);
    inline double& GetDP(int nodel, int noder, bool s);

    atomic<uint64_t> ax;
    mutex qmutex;
    queue<int> Q;
    vi PA;
    vector<vd> DP;
};

#endif
