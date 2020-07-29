/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#include "Graph.h"
#include <algorithm>
#include <fstream>
#include <cassert>
#include <limits>
#define die() assert(false)

Graph::Graph(const char* filename, const char* mapname) : network(nullptr), root(-1)
{
    ifstream file(filename), mapfile(mapname);
    if (!file || !mapfile)
        die();

    // map string indices to integers
    int node_idx; string node_name;
    while (mapfile >> node_idx >> node_name)
        m[node_name] = node_idx;

    adj.resize(m.size());
    radj.resize(m.size());
    n = adj.size();

    // create reverse mapping
    rm.resize(n);
    for (auto [name, idx] : m)
        rm[idx] = name;

    vector<int> nroot(n), nleaf(n);

    // read from file
    string sa, sb, type;
    while (file >> sa >> sb >> type)
    {
        if (type != "default")
            die();

        // directed edge sb->sa
        int x = m[sa], y = m[sb];
        adj[y].push_back(x);
        radj[x].push_back(y);
        nroot[x] = 1;
        nleaf[y] = 1;
    }

    // find root and leaves
    for (uint i = 0; i < m.size(); ++i)
    {
        // found the root
        if (!nroot[i])
        {
            // multiple roots
            if (root != -1)
                die();

            root = i;
        }
        // found a leaf
        if (!nleaf[i])
            L.push_back(i);
        N.push_back(i); // TODO: topsort
    }

    // there is no root
    if (root == -1)
        die();

    vb C(n);
    TransitiveReduction(root, C);
    bool is_tree = true;
    for (uint i = 0; i < m.size(); ++i)
        if (radj[i].size() > 1)
            is_tree = false;

    if (!is_tree)
        network = new AntichainNetwork(*this);
}

Graph::~Graph()
{
    delete network;
}

void Graph::TransitiveReduction(int node, vb& C)
{
    if (C[node])
        return;

    C[node] = true;
    for (int child : children(node))
    {
        TransitiveReduction(child, C);
        for (int cchild : children(child))
        {
            vb CC(GetNumNodes());
            TransitiveReduction(node, cchild, CC);
        }
    }
}

void Graph::TransitiveReduction(int parent, int node, vb& C)
{
    C[node] = true;
    //Reduce(&parent->child, node);
    //Reduce(&node->parent, parent);
    for (int child : children(node))
        if (!C[child])
            TransitiveReduction(parent, child, C);
}

AntichainNetwork::AntichainNetwork(Graph& graph) : graph(graph)
{
    int n = graph.GetNumNodes();
    size_t SZ = n * 2 + 2;
    G.resize(SZ);
    for (int j = 0; j < NR_THREADS; ++j)
        R[j].resize(SZ, vd(SZ));

    int S = SZ - 2, T = SZ - 1;
    for (int i = 0; i < n; ++i)
    {
        G[S].push_back(i);
        G[i].push_back(S);
        G[T].push_back(i + n);
        G[i + n].push_back(T);
    }

    vi VT;
    GenPaths(graph.GetRoot(), VT);
    sort(P.begin(), P.end(), [](const vi& a, const vi& b)
    {
        return a.size() > b.size();
    });
    vector<vb> D(n, vb(n));
    vector<vi> Q;
    for (int k = 0; k < P.size(); ++k)
    {
        ForeachPair(P[k], [&](vi& p, int i, int j)
        {
            if (D[i][j]) return;
            Q.push_back(p);
            ForeachPair(P[k], [&](vi& p, int i, int j)
            {
                D[i][j] = D[j][j] = true;
            });
        });
    }
    P = move(Q);
}

void AntichainNetwork::GenPaths(int node, vi& T)
{
    T.push_back(node);
    if (graph.children(node).empty())
        P.push_back(T);
    for (int child : graph.children(node))
        GenPaths(child, T);
    T.pop_back();
}

void Graph::TransitiveClosure()
{
    D.resize(n, vb(n));
    vector<vb> C(n, vb(n));
    for (int leaf : L)
        TransitiveClosure(leaf, leaf, C);
}

void Graph::TransitiveClosure(int node, int rnode, vector<vb>& C)
{
    int l = rnode;
    int i = node;
    if (l != i)
    {
        D[l][i] = true;
        network->AddEdge(l, i);
    }

    C[i][l] = true;
    for (int parent : parents(node))
    {
        int pn = parent;
        int pnt = pn;
        if (!C[pnt][l])
            TransitiveClosure(pn, rnode, C);
        if (!C[pnt][pnt])
            TransitiveClosure(pn, pn, C);
    }
}

void AntichainNetwork::AddEdge(int l, int i)
{
    int n = graph.GetNumNodes();
    for (int j = 0; j < NR_THREADS; ++j)
        R[j][l][i + n] = numeric_limits<double>::infinity();
    G[l].push_back(i + n);
    G[i + n].push_back(l);
}
