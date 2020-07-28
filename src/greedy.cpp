/*
    Copyright (C) 2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include <iostream>
#include <vector>
#include <map>
#include <limits>
#include <fstream>
#include <cassert>
#include "read_csv.h"
#include "array2d.h"
#define die() assert(false)
#define dbg(x) cerr << #x << " = " << x << endl
using namespace std;

struct graph
{
    // mapping of node identifiers to nodes
    map<string, int> m;
    // mapping of nodes to node identifiers
    vector<string> rm;
    // adjacency list of a node
    vector<vector<int>> adj;
    // number of nodes
    int n;
    // root node
    int r;

    graph(const char* filename, const char* mapname) : r(-1)
    {
        ifstream file(filename), mapfile(mapname);
        if (!file || !mapfile)
            die();

        // map string indices to integers
        int node_idx; string node_name;
        while (mapfile >> node_idx >> node_name)
            m[node_name] = node_idx;

        adj.resize(m.size());
        n = adj.size();

        // create reverse mapping
        rm.resize(n);
        for (auto [name, idx] : m)
            rm[idx] = name;

        vector<int> nroot(n);

        // read from file
        string sa, sb, type;
        while (file >> sa >> sb >> type)
        {
            if (type != "default")
                die();

            // directed edge sb->sa
            int x = m[sa], y = m[sb];
            adj[y].push_back(x);
            nroot[x] = 1;
        }

        // find root
        for (uint i = 0; i < m.size(); ++i)
        {
            // found the root
            if (!nroot[i])
            {
                // multiple roots
                if (r != -1)
                    die();

                r = i;
            }
        }

        // tree has no root
        if (r == -1)
            die();
    }
};

// transitive closure auxiliary
void trans_closure(const graph& t, int x, int r, array2d& D)
{
    D(r, x) = 1;
    for (int y : t.adj[x])
        trans_closure(t, y, r, D);
}

// fill transitive closure table
void make_trans(const graph& t, int x, array2d& D)
{
    trans_closure(t, x, x, D);
    for (int y : t.adj[x])
        make_trans(t, y, D);
}

// return true iff edges a and b can simultaneously be in an arboreal matching
bool valid(pair<int, int> a, pair<int, int> b, array2d& D1, array2d& D2)
{
    int i = get<0>(a), j = get<0>(b);
    int x = get<1>(a), y = get<1>(b);
    bool c2 = (D1(j, i) || D1(i, j)) == (D2(x, y) || D2(y, x));
    bool c1 = (D1(i, j) == D2(x, y)); // assuming c2 is satisfied
    bool c0 = (i != j && x != y);
    return c0 && c1 && c2;
}

int main(int argc, char** argv)
{
    if (argc != 6)
    {
        cout << "usage: " << argv[0] << " <graph1> <map1> <graph2> <map2> <matrix>\n";
        return 0;
    }
    // load the data
    graph t1(argv[1], argv[2]);
    graph t2(argv[3], argv[4]);

    // ancestry relationship tables
    array2d D1(t1.n, t1.n, 0), D2(t2.n, t2.n, 0);
    make_trans(t1, t1.r, D1);
    make_trans(t2, t2.r, D2);

    vector<vector<double>> cost_matrix = CSVReader(argv[5]).getDoubleData();

    // last row/column is deletion cost
    array2d minmatrix(t1.n + 1, t2.n + 1);

    // convert scores to integers
    const double scale = 1000;
    for (int i = 0; i < t1.n + 1; ++i)
        for (int j = 0; j < t2.n + 1; ++j)
            minmatrix(i, j) = scale * cost_matrix[i][j];

    // convert into maximization problem
    array2d maxmatrix(t1.n, t2.n);
    vector<pair<int, int>> edges;
    for (int i = 0; i < t1.n; ++i)
    {
        for (int j = 0; j < t2.n; ++j)
        {
            maxmatrix(i, j) = minmatrix(i, t2.n) + minmatrix(t1.n, j) - minmatrix(i, j);
            edges.emplace_back(i, j);
        }
    }

    // sort edges by weight
    sort(edges.begin(), edges.end(), [&](const pair<int, int>& a, const pair<int, int>& b)
    {
        auto [p, q] = a;
        auto [r, s] = b;
        return maxmatrix(p, q) > maxmatrix(r, s);
    });

    // greedily add edges to the solution
    vector<pair<int, int>> matching;
    auto can_add = [&](const pair<int, int>& a)
    {
        for (auto e : matching)
            if (!valid(a, e, D1, D2))
                return false;
        return true;
    };
    for (auto e : edges)
        if (can_add(e))
            matching.push_back(e);

    // weight of recovered matching
    int weight = 0;
    for (auto [x, y] : matching)
        weight += maxmatrix(x, y);

    // scale back the score
    clog << weight / scale << endl;

    // print the matching
    for (auto [x, y] : matching)
        cout << t1.rm[x] << ' ' << t2.rm[y] << '\n';
}
