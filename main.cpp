#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <cstddef>
#include <mem.h>
#include "VirtualDisk.h"


using namespace std;
using namespace boost::algorithm;

vector<vector<string>> readDatafile(string fileDirectory) {
    vector<vector<string>> dataset;
    string line;
    ifstream MyReadFile(fileDirectory);

    getline(MyReadFile, line); //Read Header Line

    while (getline(MyReadFile, line)) {
        // Split line into tab-separated parts
        vector<string> parts;
        split(parts, line, boost::is_any_of("\t"));
        dataset.push_back(parts);
    }
    // Close the file
    MyReadFile.close();
    return dataset;
}


int main() {
    string fileDirectory = "D:\\Projects\\Database\\data\\data_100.tsv";

    vector<tuple<uchar, uchar, size_t>> dataFormat;
    dataFormat.push_back(make_tuple(1, 's', 9));
    dataFormat.push_back(make_tuple(2, 'f', 1));
    dataFormat.push_back(make_tuple(3, 'i', 2));

    vector<vector<string>> rawData = readDatafile(fileDirectory);

    // Create virtual disk of 50MB, block size 100B
    VirtualDisk virtualDisk(50000000,100, 0.20);

    for(int i=0;i<rawData.size();i++){
        virtualDisk.addRecord(dataFormat, rawData[i]);
    }

    virtualDisk.reportStats();

    return 0;
}