#include "read_csv.h"

vector<vector<double> > CSVReader::getDoubleData(int from_row, int to_row)
{
    vector<vector<double> > data;
    ifstream inputFile(fileName);
    int l = 0;
    int count =0;
 
    while (inputFile) {
        if (count >= to_row) break;
        l++;
        string s;
        if (!getline(inputFile, s)) break;
        if (s[0] != '#') {
            istringstream ss(s);
            vector<double> record;
            int removefist = 0;
            
            while (ss) {
                string line;
                if (!getline(ss, line, ','))
                {
                    break;
                }
                try {
                    if (removefist >= 0){ // matrix
                        record.push_back(stod(line));
                    }    
                    
                    else
                        removefist++;
                }
                catch (const invalid_argument e) {
                    cout << "NaN found in file " << fileName << " line " << l
                         << endl;
                    e.what();
                }
            }
            if (count >= from_row)
                data.push_back(record);
            count ++;
        }
    }
 
    if (!inputFile.eof()) {
        cerr << "Could not read file " << fileName << "\n";
        __throw_invalid_argument("File not found.");
    }
 
    return data;
}
