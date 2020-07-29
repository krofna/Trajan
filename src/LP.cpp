/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include "LP.h"
#include "Timer.h"
#include <iostream>
#include <fstream>

LP::LP(Graph& t1, Graph& t2, vector<vd>& matrix) : t1(t1), t2(t2), matrix(matrix), c(t1.GetNumNodes() * t2.GetNumNodes()), nr_rows(0), nr_cols(0)
{
    K.resize(t1.GetNumNodes(), vi(t2.GetNumNodes(), -1));
    MatchingConstraints();
    dag = t1.GetNetwork() || t2.GetNetwork();
}

LP::~LP()
{
}

bool LP::IsNotInConflict(int i, int j, int x, int y) const
{
    if (i == j || x == y) return false;
    bool c2 = (t1.D[j][i] || t1.D[i][j]) == (t2.D[x][y] || t2.D[y][x]);
    bool c1 = (t1.D[i][j] == t2.D[x][y]); // assuming c2 is satisfied
    return c2 && c1;
}

void LP::MatchingConstraints()
{
    cnt = 0;
    vb P(t1.GetNumNodes());
    int n = t1.GetNumNodes(), m = t2.GetNumNodes();
    for (int i : t1.nodes())
    {
        for (int j : t2.nodes())
        {
            double w = matrix[i][j];
            if (w != 0)
            {
                int col = i * m + j - cnt;
                K[i][j] = col;
                Triplets.emplace_back(i, col, 1.);
                Triplets.emplace_back(n + j, col, 1.);
                c(col) = w;
            }
            else
                ++cnt;
        }
    }
    nr_rows = n + m;
    nr_cols = n * m - cnt;
    c.conservativeResize(nr_cols);
    // set warm_x and warm_y initialy to zeroes
    warm_x = Vector::Zero(nr_cols);
    warm_y = Vector::Zero(nr_rows);
}

void LP::Solve(string filename)
{
    int cnt = 1;
    for (int i = 0; cnt; i++)
    {
        Timer T_lp, T_cross, T_indep;
        T_lp.start();
        LP::SolveLP();
        WriteSolution(filename);
        T_lp.stop();
        clog << ">>> Time for solve: \t\t" << T_lp.secs() << " secs" << endl;

        T_cross.start();
        cnt = Add<1>();
        T_cross.stop();
        clog << ">>> Time for crossing constraints: \t\t" << T_cross.secs() << " secs" << endl;

        T_indep.start();
        cnt += Add<2>();
        T_indep.stop();
        clog << ">>> Time for independent set constraints: \t\t" << T_indep.secs() << " secs" << endl;

        clog << "Added " << cnt << " rows." << endl;
    }
}

void LP::SolveLP()
{
    clog << "nr_rows = " << nr_rows << " and nr_cols = " << nr_cols << endl;

    SpMat A(nr_rows, nr_cols);
    A.setFromTriplets(Triplets.begin(), Triplets.end());
    SpMat A_t = A.transpose();
    Vector b = Vector::Ones(nr_rows);

    x = warm_x;
    //x = Vector::Zero(nr_cols);
    y = Vector::Zero(nr_rows);

    Vector c1 = -c;
    PackingJRF simpleJRF(A, b, c1, warm_x, y);
    AugmentedLagrangian solver(simpleJRF, 15);
    solver.setParameter("verbose", false);
    solver.setParameter("pgtol", 1e-1); // should influence running time a lot
    solver.setParameter("constraintsTol", 1e-3);
    Timer timeGeno;
    timeGeno.start();
    solver.solve();
    timeGeno.stop();

    clog << "f = " << solver.f() << " computed in time: " << timeGeno.secs() << " secs" << endl;

    warm_x = x = Vector::ConstMapType(solver.x(), nr_cols);
    y = Vector::ConstMapType(solver.y(), nr_rows);
}

void LP::WriteSolution(string fileName)
{
    ofstream sol_file(fileName);
    double weight = 0;
    vector<int> n1 = t1.nodes(), n2 = t2.nodes();
    for (size_t i = 0; i < K.size(); i++)
    {
        for (size_t j = 0; j < K[i].size(); j++)
        {
            if (K[i][j] != -1 && x(K[i][j]) > 0)
            {
                weight += x(K[i][j]) * c(K[i][j]);
                sol_file << t1.label(n1[i]) << " " << t2.label(n2[j]) << " " << x(K[i][j]) << "\n";
            }
        }
    }
    ofstream trp_file(fileName + ".trp");
    for (auto& t : Triplets)
        trp_file << t.row() << " " << t.col() << " " << t.value() << '\n';
    cout << weight << endl;
}
