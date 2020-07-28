/*
    Copyright (C) 2019-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef ARRAY2D
#define ARRAY2D

#include <vector>
#include <limits>
using namespace std;

constexpr int NINF = numeric_limits<int>::min();

struct array2d
{
    int n, m;
    vector<int> v;

    array2d() : n(0), m(0)
    {
    }

    array2d(int n, int m, int val = NINF) : n(n), m(m)
    {
        v.resize(n * m, val);
    }

    int& operator()(int x, int y)
    {
        return v[x * m + y];
    }

    const int& operator()(int x, int y) const
    {
        return v[x * m + y];
    }

    array2d transpose() const
    {
        array2d D(m, n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < m; ++j)
                D(j, i) = (*this)(i, j);
        return D;
    }
};

#endif
