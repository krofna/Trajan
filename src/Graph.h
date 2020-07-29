/*
    Copyright (C) 2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <map>
using namespace std;

typedef vector<int> vi;
typedef vector<double> vd;
typedef vector<bool> vb;

class AntichainNetwork;

const size_t NR_THREADS = 4;

class Graph
{
public:
    Graph(const char* filename, const char* mapname);
    ~Graph();

    int GetNumNodes() const { return n; }
    int GetRoot() const { return root; }
    AntichainNetwork* GetNetwork() { return network; }
    const vi& children(int node) const { return adj[node]; }
    const vi& parents(int node) const { return radj[node]; }
    const vi& leaves() const { return L; }
    const vi& nodes() const { return N; }
    string label(int node) const { return rm[node]; }

private:
    void TransitiveReduction(int node, vb& C);
    void TransitiveReduction(int parent, int node, vb& C);

    // mapping of node identifiers to nodes
    map<string, int> m;
    // mapping of nodes to node identifiers
    vector<string> rm;
    // adjacency list of a node
    vector<vi> adj;
    // reverse adjacency list of a node
    vector<vi> radj;
    // leaves/nodes
    vi L, N;
    // root node
    int root;
    // number of nodes
    int n;

    AntichainNetwork* network;
};

class AntichainNetwork
{
public:
    AntichainNetwork(Graph& graph);

    vector<vi> G;
    vector<vd> R[NR_THREADS];
    vector<vi> P;

    void TransitiveClosure(int node, int rnode, vector<vb>& C);

private:
    void GenPaths(int node, vi& P);

    template<class F>
    void ForeachPair(vi& Q, F f)
    {
        for (int i = 0; i < Q.size(); ++i)
            for (int j = i + 1; j < Q.size(); ++j)
                f(Q, Q[i], Q[j]);
    }

    Graph& graph;
};

#endif
