/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include "Constraint.h"

Constraint::Constraint(MPSolver& solver, vector<ET>& Triplets, vector<MPConstraint*>& constraints, vector<MPVariable*>& variables, Graph& t1, Graph& t2, vector<vi>& K, Vector& x, bool swp) : solver(solver), Triplets(Triplets), constraints(constraints), variables(variables), t1(t1), t2(t2), K(K), x(x), swp(swp)
{
}

void Constraint::AddConstraint(int row, vii& P)
{
    constraints.push_back(solver.MakeRowConstraint(-solver.infinity(), 1));
    for (auto [u, v] : P)
    {
        int col = GetCol(u, v);
        if (col != -1)
        {
            Triplets.emplace_back(row, col, 1.);
            constraints[row]->SetCoefficient(variables[col], 1);
        }
    }
}
