/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef INDEPENDENT_SET_CONSTRAINT_H
#define INDEPENDENT_SET_CONSTRAINT_H

#include "Constraint.h"

typedef list<int> LN;
typedef pair<double, LN> dLN;

class IndependentSetConstraint : Constraint
{
public:
    IndependentSetConstraint(vector<ET>& Triplets, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp);

    int AddTriplets(int nr_rows);
private:
    void DFSRight(int noder);
    void DFSLeft(int nodel, int noder, double w);
    dLN DFSRight(int nodel, int noder);

    vector<vd> D;
};

#endif
