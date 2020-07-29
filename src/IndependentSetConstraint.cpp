/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include "IndependentSetConstraint.h"

IndependentSetConstraint::IndependentSetConstraint(vector<ET>& Triplets, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp) : Constraint(Triplets, t1, t2, K, x, swp), D(t1.GetNumNodes(), vd(t2.GetNumNodes()))
{
}

int IndependentSetConstraint::AddTriplets(int nr_rows)
{
    for (int node : t2.nodes())
        DFSLeft(t1.GetRoot(), node, 0);

    int ncr = 0;
    for (int node : t1.leaves())
    {
        dLN L = DFSRight(node, t2.GetRoot());
        if (L.first - EPS <= 1)
            continue;

        vii P;
        for (int noder : L.second)
            for (int nodel = node; nodel != -1; nodel = (t1.parents(nodel).empty() ? -1 : t1.parents(nodel)[0]))
                P.emplace_back(nodel, noder);

        AddConstraint(nr_rows + ncr++, P);
    }
    return ncr;
}

void IndependentSetConstraint::DFSLeft(int nodel, const int noder, double w)
{
    if (t1.children(nodel).empty())
        D[nodel][noder] = w + GetWeight(nodel, noder);
    for (int child : t1.children(nodel))
        DFSLeft(child, noder, w + GetWeight(nodel, noder));
}

dLN IndependentSetConstraint::DFSRight(const int nodel, int noder)
{
    double w = D[nodel][noder], sum = 0;
    LN V;

    for (int child : t2.children(noder))
    {
        auto [ww, T] = DFSRight(nodel, child);
        sum += ww;
        V.splice(V.begin(), T);
    }

    if (sum > w)
        return {sum, V};
    return {w, LN(1, noder)};
}
