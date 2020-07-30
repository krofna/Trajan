/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include "CrossingConstraint.h"
#include <thread>

CrossingConstraint::CrossingConstraint(vector<ET>& Triplets, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp) : Constraint(Triplets, t1, t2, K, x, swp), DP(t1.GetNumNodes(), vd(t2.GetNumNodes(), -1)), PA(t1.GetNumNodes())
{
    vb C(t1.GetNumNodes());
    DFSLeft(t1.GetRoot(), C);
    Q.push(t1.GetRoot());
    RunParallel();
}

int CrossingConstraint::AddTriplets(int nr_rows)
{
    int ncr = 0;
    for (int node : t1.leaves())
    {
        vii P;
        Reconstruct(P, node, t2.GetRoot());

        double sum = 0;
        for (auto [u, v] : P)
            sum += GetWeight(u, v);

        if (sum - EPS > 1)
            AddConstraint(nr_rows + ncr++, P);
    }
    return ncr;
}

void CrossingConstraint::RunParallel()
{
    const size_t NR_THREADS = min(::NR_THREADS, size_t(64));
    ax = (NR_THREADS < 64 ? (uint64_t(1) << NR_THREADS) - 1 : ~0);
    vector<thread> vt;
    for (int i = 0; i < NR_THREADS; ++i)
        vt.emplace_back(&CrossingConstraint::CrossingJob, this, i);

    for (int i = 0; i < NR_THREADS; ++i)
        vt[i].join();
}

void CrossingConstraint::CrossingJob(int i)
{
    while (ax)
    {
        int node = -1;
        {
            lock_guard<mutex> g(qmutex);
            if (Q.empty())
            {
                ax &= ~(1ll << i);
                continue;
            }
            else
                ax |= 1ll << i;
            node = Q.front();
            Q.pop();
        }
        DFSRight(t2.GetRoot(), node);
        lock_guard<mutex> g(qmutex);
        for (int child : t1.children(node))
            if (--PA[child] == 0)
                Q.push(child);
    }
}

pair<int, double> CrossingConstraint::GetMaxPC(int nodel, const vi& noder, bool s)
{
    double mx = 0;
    int mc = -1;
    for (int pc : noder)
    {
        double cw = GetDP(nodel, pc, s);
        if (cw >= mx)
        {
            mx = cw;
            mc = pc;
        }
    }
    return {mc, mx};
}

inline double& CrossingConstraint::GetDP(int nodel, int noder, bool s = false)
{
    return s ? DP[noder][nodel] : DP[nodel][noder];
}

inline pair<int, double> CrossingConstraint::GetMaxChild(int nodel, int noder)
{
    return GetMaxPC(nodel, t2.children(noder), false);
}

inline pair<int, double> CrossingConstraint::GetMaxParent(int nodel, int noder)
{
    return GetMaxPC(nodel, t2.parents(noder), true);
}

void CrossingConstraint::DFSLeft(int node, vb& C)
{
    C[node] = true;
    for (int child : t1.children(node))
    {
        PA[child]++;
        if (!C[child])
            DFSLeft(child, C);
    }
}

double CrossingConstraint::DFSRight(int node, int nodel)
{
    double& sol = GetDP(nodel, node);
    if (sol != -1)
        return sol;

    double mx = 0;
    for (int child : t2.children(node))
        mx = max(mx, DFSRight(child, nodel));
    mx = max(mx, GetMaxParent(node, nodel).second);
    return sol = mx + GetWeight(nodel, node);
}

void CrossingConstraint::Reconstruct(vii& P, int nodel, int noder)
{
    auto [child, cw] = GetMaxChild(nodel, noder);
    auto [parent, pw] = GetMaxParent(noder, nodel);
    P.emplace_back(nodel, noder);
    if (parent != -1 && (child == -1 || pw > cw))
        Reconstruct(P, parent, noder);
    else if (child != -1 && (parent == -1 || cw >= pw))
        Reconstruct(P, nodel, child);
    else
        assert(parent == -1 && child == -1);
}
