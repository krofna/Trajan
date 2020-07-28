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
#include "LPInt.h"
#include "LPFInt.h"
#include "read_csv.h"

LP* MakeSolver(Graph& t1, Graph& t2, int s)
{
    if (s == 0)
        return new LP(t1, t2);
    else if (s == 2)
        return new LPInt(t1, t2);
    else if (s == 3)
        return new LPFInt(t1, t2);
    return nullptr;
}

pair<Graph*, Graph*> MakeGraphs(char** argv)
{
    return {new Graph(argv[1], argv[2]), new Graph(argv[3], argv[4])};
}

int main(int argc, char** argv)
{
    if (argc != 8)
    {
        cout << "usage: " << argv[0] << " <graph1> <map1> <graph2> <map2> <matrix> <align> <solver>" << endl;
        return EXIT_FAILURE;
    }

    Timer T;
    T.start();

    auto [t1, t2] = MakeGraphs(argv);
    vector<vector<double>> cost_matrix = CSVReader(argv[5]).getDoubleData();
    LP* solver = MakeSolver(*t1, *t2, stoi(argv[7]));
    if (solver) solver->Solve(argv[6]);
    T.stop();
    clog << "TIME: " << T.secs() << " secs" << endl;

    delete solver;
    delete t1;
    delete t2;
}
