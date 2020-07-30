#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "read_csv.h"

vector<vector<double>> ReadCSV(string filename)
{
    vector<vector<double>> data;
    ifstream inputFile(filename);

    while (inputFile)
    {
        string s;
        if (!getline(inputFile, s)) break;
        if (s[0] == '#') continue;

        istringstream ss(s);
        vector<double> record;
        while (ss)
        {
            string line;
            if (!getline(ss, line, ','))
                break;

            record.push_back(stod(line));
        }
        data.push_back(record);
    }

    if (!inputFile.eof())
        __throw_invalid_argument("File not found.");

    return data;
}
