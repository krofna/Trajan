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
        int removefist = 0;
        while (ss)
        {
            string line;
            if (!getline(ss, line, ','))
                break;

            try
            {
                if (removefist >= 0)
                    record.push_back(stod(line));
                else
                    removefist++;
            }
            catch (const invalid_argument e) {
                cout << "NaN found in file" << endl;
            }
        }
        data.push_back(record);
    }

    if (!inputFile.eof())
    {
        cerr << "Could not read file " << filename << "\n";
        __throw_invalid_argument("File not found.");
    }

    return data;
}
