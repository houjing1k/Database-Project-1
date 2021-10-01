#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <string>
#include <sstream>
#include <vector>
#include <tuple>
#include <boost/algorithm/string.hpp>
#include <cstddef>
#include <mem.h>
#include <chrono>
#include <ctime>
#include "VirtualDisk.h"
#include "BPTree.h"


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
    string fileDirectory = "..\\data\\data_tree_3.tsv";

    // Data Format: uchar fieldID, uchar dataType, size_t dataSize
    vector<tuple<uchar, uchar, size_t>> dataFormat;
    dataFormat.push_back(make_tuple(1, 's', 9));
    dataFormat.push_back(make_tuple(2, 'f', 1));
    dataFormat.push_back(make_tuple(3, 'i', 3));

    // Read and parse tsv
    vector<vector<string>> rawData = readDatafile(fileDirectory);

    // Create virtual disk of 50MB, block size 100B
    VirtualDisk virtualDisk(50000000, 100, 0.20);

    // Create B+Tree Index of node size 3
    BPTree bpTree(3);

    // Add datasets to database
    cout << "Adding " << rawData.size() << " records to database..." << endl;
    bool success = true;
    int reportInterval = 1000;
    vector<tuple<uint, void *, uint_s>> mappingTable;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now(); // Start timer
    std::chrono::steady_clock::time_point end;
    uint totalTime = 0;
    for (int i = 0; i < rawData.size(); i++) {
        // Report Stats
        if (i != 0 && i % reportInterval == 0) {
            end = std::chrono::steady_clock::now(); // Stop timer
            uint elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(end - begin).count();
            totalTime += elapsedTime;
            uint etc = totalTime / (i / reportInterval) * (rawData.size() - (i + 1)) / reportInterval / 60;
            cout << "Added [" << i << "] records. [" << rawData.size() - i << "] records left. \t[" << fixed
                 << setprecision(2)
                 << (float) i * 100 / (float) rawData.size() << "%]" << endl;
            cout << "Elapsed time: " << elapsedTime << "[s]. ETC: " << etc << "[min]" << std::endl;
            begin = std::chrono::steady_clock::now(); // Start timer
        }

        // Add Record
        // <recordID, pBlk, recordNum>
        tuple<uint, void *, uint_s> dataMap = virtualDisk.addRecord(dataFormat, rawData[i]);
        if (get<0>(dataMap) != -1) { // If insertion success
            mappingTable.push_back(dataMap); // add dataMap to mappingTable
            int key = stoi(rawData[i][2]); //define key
            cout << "adding bptree key: " << key << endl;
            bpTree.insertKey(key, &mappingTable[i]);
            bpTree.printTree(bpTree.rootNode);
        } else {
            success = false;
            break;
        }
    }
    if (success) {
        cout << "Successfully added " << mappingTable.size() << " records" << endl;
//        bpTree.printTree();
    } else {
        cout << "Operation aborted. Insertion error." << endl;
    }

    virtualDisk.reportStats();

//     Fetch Record
//    cout << "Fetching rec1" << endl;
//    vector<tuple<uchar, string>> rec1 = virtualDisk.fetchRecord(mappingTable[0]);
//    for (int i = 0; i < rec1.size(); i++) {
//        cout << "Field ID: " << (int) get<0>(rec1[i]) << endl;
//        cout << "Data: " << get<1>(rec1[i]) << endl;
//    }

    // Delete Record
//    virtualDisk.deleteRecord(mappingTable[6]);
//    virtualDisk.deleteRecord(mappingTable[7]);
//    virtualDisk.deleteRecord(mappingTable[8]);
//
//    virtualDisk.reportStats();


    system("pause");
    return 0;
}