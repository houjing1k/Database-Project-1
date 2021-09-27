//
// Created by jingh on 20/9/2021.
//

#ifndef DATABASE_VIRTUALDISK_H
#define DATABASE_VIRTUALDISK_H

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;

using namespace std;

class VirtualDisk {
private:
    uchar* pDisk;

    vector<bool> blkValidity;

    uint diskSize;
    uint blkSize;
    uint blkHeaderSize;
    uint numMaxRecPerBlk;

    uint numTotalBlk;
    uint numFreeBlk;
    uint numAllocBlk;


public:
    VirtualDisk(uint diskSize, uint blockSize, float blkHeaderRatio);

    int allocBlk();

    bool deallocBlk(int blkOffset);

    tuple<uchar *, uint> addRecord(vector<tuple<uchar, uchar, size_t>> dataFormat, vector<string> data);

    bool deleteRecord(tuple<uchar *, uint> recordDirectory);

    bool fetchRecord(tuple<uchar *, uint> recordDirectory);

    ~VirtualDisk();

    void intToBytes(uint integer, uchar *bytes);
    void fixedStringToBytes(string s, uint length, uchar *bytes);
    void floatToBytes(float f, uchar *bytes);
    void packToField(string data, uchar fieldID, uchar type, size_t dataSize, uchar *field);
    uint packToRecord(vector<uchar *> fields, uchar numFields, uchar *record);
    int insertRecordToBlock(uchar *targetBlock, size_t blockSize, uchar *targetRecord, int recordSize);

    void printHex(uchar *target, size_t size, string label);
    void reportStats();
};


#endif //DATABASE_VIRTUALDISK_H
