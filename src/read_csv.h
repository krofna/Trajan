#ifndef READCSV_H
#define READCSV_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <algorithm>
using namespace std;

class CSVReader
{
	string fileName;
	string delimeter;

public:
	CSVReader(string filename, string delm = ",") :
			fileName(filename), delimeter(delm)
	{ }

    vector<vector<double> > getDoubleData(int from_row = 0, int to_row = 10000);
};

#endif
