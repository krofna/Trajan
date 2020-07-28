/*
    Copyright (C) 2018-2020 Mislav Blažević

    This file is part of dagmatch.

    dagmatch is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
*/
#ifndef LPFINT_H
#define LPFINT_H

#include "LPInt.h"

class LPFInt : public LPInt
{
public:
    LPFInt(Graph& t1, Graph& t2);

    virtual void Solve(string filename) override;
};

#endif
