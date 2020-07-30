/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <iostream>
#include "Timer.h"
#include "LP.h"
#include "read_csv.h"

int main(int argc, char** argv)
{
    if (argc != 8)
    {
        cout << "usage: " << argv[0] << " <graph1> <map1> <graph2> <map2> <matrix> <align> <solver>" << endl;
        return EXIT_FAILURE;
    }

    Timer T;
    T.start();

    // read dags
    Graph t1(argv[1], argv[2]), t2(argv[3], argv[4]);
    // read (minimization) weights
    vector<vd> minmatrix = ReadCSV(argv[5]);
    // convert into maximization problem
    vector<vd> maxmatrix(t1.GetNumNodes(), vd(t2.GetNumNodes()));
    for (int i = 0; i < t1.GetNumNodes(); ++i)
        for (int j = 0; j < t2.GetNumNodes(); ++j)
            maxmatrix[i][j] = minmatrix[i][t2.GetNumNodes()] + minmatrix[t1.GetNumNodes()][j] - minmatrix[i][j];

    LP solver(t1, t2, maxmatrix);
    string output = argv[6];
    switch (stoi(argv[7]))
    {
        case 0:
            solver.Solve(output, false);
            break;
        case 1:
            solver.Solve(output, false);
            solver.Solve(output, true);
            break;
        case 2:
            solver.Solve(output, true);
            break;
        default:
            assert(false);
    }

    T.stop();
    clog << "TIME: " << T.secs() << " secs" << endl;
}
