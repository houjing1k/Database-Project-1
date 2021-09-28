//
// Created by jingh on 20/9/2021.
//

#include <bits/stdc++.h>
#include "VirtualDisk.h"
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;

const bool DEBUG_MODE = false;

using namespace std;

// Constructor Method
VirtualDisk::VirtualDisk(uint diskSize, uint blkSize, float blkHeaderRatio) {
    this->blkSize = blkSize;
    this->diskSize = diskSize;
    this->numTotalBlk = diskSize / blkSize;
    this->numFreeBlk = numTotalBlk;
    this->numAllocBlk = 0;
    this->numMaxRecPerBlk = (int) (blkSize / 5 * blkHeaderRatio);
    this->blkHeaderSize = numMaxRecPerBlk * 5 + 3;
    this->curRecordID = 0;

    this->pDisk = (uchar *) calloc(diskSize, 1);
    if (this->pDisk == nullptr) {
        cout << "Virtual Disk Creation Failed: Calloc Error" << endl;
    } else {
        for (int i = 0; i < numTotalBlk; i++) blkValidity.push_back(false);

        cout << "New Virtual Disk Created." << endl;
        reportStats();
    }
}

int VirtualDisk::allocBlk() {
    if (numFreeBlk < 1) {
        cout << "Block Allocation Failed: Memory Full" << endl;
        return -1;
    } else {
        int blkOffset = 0;
        for (int i = 0; i < numTotalBlk; i++) {
            if (!blkValidity[i]) {
                blkOffset = i;
                break;
            }
        }
        uchar *pBlk = pDisk + (blkOffset * blkSize);
        if (DEBUG_MODE)
            cout << "Block Allocation Success. Offset: " << blkOffset << " Physical Address: "
                 << static_cast<void *>(pBlk)
                 << endl;

        // Init Block Array to 0
        for (int i = 0; i < blkSize; ++i) {
            pBlk[i] = 0;
        }

        // Init Block Header
        int headerOffset = 3;
        int recordPointerSize = 5;
        int freeSpace = blkSize - blkHeaderSize;
        *pBlk = blkHeaderSize;
        *(pBlk + 1) = (freeSpace >> 8) & 0xFF;
        *(pBlk + 2) = freeSpace & 0xFF;
        for (int i = headerOffset; i < blkHeaderSize; i = i + recordPointerSize) {
            *(pBlk + i) = (i / recordPointerSize) + 1;
        }

//        printHex(pBlk, blkSize, "Init Block");
//        cout << endl;

        blkValidity[blkOffset] = true;
        numAllocBlk++;
        numFreeBlk--;
        return blkOffset;
    }
}

bool VirtualDisk::deallocBlk(int blkOffset) {
    if (numAllocBlk < 1) {
        if (DEBUG_MODE) cout << "Block De-allocation Failed: No Allocated Blocks" << endl;
        return false;
    } else if (!blkValidity[blkOffset]) {
        if (DEBUG_MODE) cout << "Block De-allocation Failed: Block not allocated" << endl;
        return false;
    } else {
        if (DEBUG_MODE) cout << "Block De-allocation Success" << endl;
        blkValidity[blkOffset] = false;
        numAllocBlk--;
        numFreeBlk++;
        return true;
    }
}

void VirtualDisk::writeBlock(uchar *pBlk, uchar *blockCpy) {
    uint freeSpace = (blockCpy[1] << 8) | blockCpy[2];
    if (DEBUG_MODE) cout << "Free space: " << freeSpace << endl;
    if (freeSpace == (blkSize - blkHeaderSize)) {
        cout << "Block is empty. Deallocating Block." << endl;
        deallocBlk((int) (pBlk - pDisk) / blkSize);
    } else memcpy(pBlk, blockCpy, blkSize);
}

uchar *VirtualDisk::readBlock(uchar *pBlk) {
    uchar *blockCpy = (uchar *) calloc(blkSize, 1);
    memcpy(blockCpy, pBlk, blkSize);
    return blockCpy;
}


tuple<uint, void *, uint_s>
VirtualDisk::addRecord(vector<tuple<uchar, uchar, size_t>> dataFormat, vector<string> data) {
    if (DEBUG_MODE) cout << "Add Record" << endl;

    // Init return variables
    uint recordID = curRecordID;
    uchar *pBlk = nullptr;
    int recordNum = -1;

    int blkOffset = -1;
    size_t recordSize = 0;
    recordSize++; // Add record header (1B)

    // Pack data to fields and record
    int numOfField = dataFormat.size();
    vector<uchar *> fields;
//    cout << "no. of field: " << numOfField << endl;
    for (int i = 0; i < numOfField; i++) {
        uchar fieldID = get<0>(dataFormat[i]);
        uchar dataType = get<1>(dataFormat[i]);
        size_t dataSize = get<2>(dataFormat[i]);
        size_t fieldSize = dataSize + 3;
        recordSize = recordSize + fieldSize;
        uchar *field = (uchar *) calloc(fieldSize, 1);
        packToField(data[i], fieldID, dataType, dataSize, field);
        fields.push_back(field);
//        printHex(field, fieldSize, to_string(fieldID));
    }
    uchar record[recordSize];
    packToRecord(fields, numOfField, record);
//    printHex(record, recordSize, "record");

    for (int i = 0; i < numOfField; i++) free(fields[i]); // Free temp calloc memory

    // Determine Block Number
    if (numAllocBlk < 1) {
        if (DEBUG_MODE) cout << "No allocated blocks." << endl;
        blkOffset = allocBlk(); // No allocated blocks
        if (blkOffset == -1)return make_tuple(-1, nullptr, -1); //Allocation Failed
    } else {
        for (int i = 0; i < numTotalBlk; i++) {
            int availSpace = (*(pDisk + (i * blkSize) + 1) << 8) | (*(pDisk + (i * blkSize) + 2));
            bool isHeaderFull = ((*(pDisk + (i * blkSize) + ((numMaxRecPerBlk - 1) * 5) + 4) << 8) |
                                 (*(pDisk + (i * blkSize) + ((numMaxRecPerBlk - 1) * 5) + 5))) != 0;
            if (blkValidity[i] && availSpace >= recordSize &&
                !isHeaderFull) { // Found an allocated block with sufficient space
                blkOffset = i;
                if (DEBUG_MODE) cout << "Block " << i << " has sufficient space." << endl;
                break;
            }
        }
        if (blkOffset == -1) {
            if (DEBUG_MODE) cout << "No space in all allocated blocks." << endl;
            blkOffset = allocBlk(); // No space in all allocated blocks
            if (blkOffset == -1)return make_tuple(-1, nullptr, -1); //Allocation Failed
        }
    }

    // Add Record to Block
    pBlk = pDisk + (blkOffset * blkSize);
    recordNum = insertRecordToBlock(pBlk, record, recordSize);
    if (recordNum == -1) {
        blkOffset = allocBlk();
        pBlk = pDisk + (blkOffset * blkSize);
        recordNum = insertRecordToBlock(pBlk, record, recordSize);
    }
    if (recordNum == -1) { // Insertion Failed
        if (DEBUG_MODE) cout << "Insertion Failed" << endl;
        return make_tuple(-1, nullptr, -1);
    } else {
        if (DEBUG_MODE) cout << "recordID: " << recordID << endl;
        if (DEBUG_MODE) cout << "Blk Addr: " << (void *) pBlk << endl;
        if (DEBUG_MODE) cout << "recordNum: " << recordNum << endl;

        printHex(pBlk, blkSize, "After add");

        curRecordID++;
        tuple<uint, void *, uint_s> recordMap = make_tuple(recordID, pBlk, recordNum);
        return recordMap;
    }
}

bool VirtualDisk::deleteRecord(tuple<uint, void *, uint_s> recordDirectory) {
    if (DEBUG_MODE) cout << "Delete Record" << endl;
    uint recordID = get<0>(recordDirectory);
    uchar *pBlk = (uchar *) get<1>(recordDirectory);
    if (DEBUG_MODE) cout << "pBlk = " << (void *) pBlk << endl;
    uint_s recordNum = get<2>(recordDirectory);

    uchar *blockCpy = readBlock(pBlk);
    printHex(blockCpy, blkSize, "before deletion");

    if (removeRecordFromBlock(blockCpy, recordNum)) {
        printHex(blockCpy, blkSize, "after deletion");
        writeBlock(pBlk, blockCpy);
        return true;
    } else return false;
}

vector<tuple<uchar, string>> VirtualDisk::fetchRecord(tuple<uint, void *, uint_s> recordDirectory) {
    if (DEBUG_MODE) cout << "Fetch Record" << endl;
    uint recordID = get<0>(recordDirectory);
    uchar *pBlk = (uchar *) get<1>(recordDirectory);
    uint_s recordNum = get<2>(recordDirectory);

    uchar *blockCpy = readBlock(pBlk);
    printHex(blockCpy, blkSize, "fetched block");
    cout << "recordID " << recordID << " recordNum " << recordNum << endl;
    vector<tuple<uchar, uchar, size_t, uchar *>> recordSet = fetchRecordFromBlock(blockCpy, recordNum);
    vector<tuple<uchar, string>> data = decodeRecord(recordSet);

    return data;
}

vector<tuple<uchar, string>> VirtualDisk::decodeRecord(vector<tuple<uchar, uchar, size_t, uchar *>> recordSet) {
    vector<tuple<uchar, string>> decodedRecord;
    for (int i = 0; i < recordSet.size(); i++) {
        uchar fieldID = get<0>(recordSet[i]);
        uchar dataType = get<1>(recordSet[i]);
        size_t dataSize = get<2>(recordSet[i]);
        uchar *data = get<3>(recordSet[i]);
        string decodedString;
        switch (dataType) {
            case 's':
                decodedString = bytesToFixedString(data, dataSize);
                break;
            case 'i':
                decodedString = to_string(bytesToInt(data, dataSize));
                break;
            case 'f':
                stringstream stream;
                stream << fixed << setprecision(1) << bytesToFloat(data);
                decodedString = stream.str();
        }
        decodedRecord.push_back(make_tuple(fieldID, decodedString));
    }
    return decodedRecord;
}

void VirtualDisk::intToBytes(uint integer, uchar *bytes) {
    int numBytes = sizeof(integer);
    for (int i = 0; i < numBytes; i++) {
        bytes[i] = (integer >> (i * 8)) & 0x00FF;
    }
}

void VirtualDisk::fixedStringToBytes(string s, uint length, uchar *bytes) {
    for (int i = 0; i < length; i++) {
        bytes[i] = s[i];
    }
}


void VirtualDisk::floatToBytes(float f, uchar *bytes) {
    *bytes = (unsigned int) (f * 10);
}

float VirtualDisk::bytesToFloat(uchar *bytes) {
    return (float) ((unsigned int) *bytes) / 10.0;
}

int VirtualDisk::bytesToInt(uchar *bytes, size_t numBytes) {
    int integer = 0;
    for (int i = 0; i < numBytes; i++) {
        integer = (integer << 8) | bytes[i];
    }
    return integer;
}

string VirtualDisk::bytesToFixedString(uchar *bytes, size_t numBytes) {
    string s = "";
    for (int i = 0; i < numBytes; i++) {
        s.push_back(bytes[i]);
    }
    return s;
}

void VirtualDisk::packToField(string data, uchar fieldID, uchar type, size_t dataSize, uchar *field) {
    field[0] = fieldID;
    field[1] = type;
    field[2] = dataSize;

    uchar dataBytes[dataSize];
    switch (type) {
        case 's':
            fixedStringToBytes(data, dataSize, dataBytes);
            break;
        case 'f':
            floatToBytes(atof(data.c_str()), dataBytes);
            break;
        case 'i':
            intToBytes(atoi(data.c_str()), dataBytes);
            break;
    }

    for (int i = 0; i < dataSize; i++) {
        field[3 + i] = dataBytes[i];
    }
}


uint VirtualDisk::packToRecord(vector<uchar *> fields, uchar numFields, uchar *record) {
    uint recordSize = 0;
    record[0] = numFields;
    unsigned char *ptr = record;
    ptr = ptr + 1;
    recordSize++;
    for (int i = 0; i < numFields; i++) {
        int fieldSize = fields[i][2] + 3;
//        cout << fieldSize << endl;
        recordSize = recordSize + fieldSize;
        for (int j = 0; j < fieldSize; j++) {
            *ptr = fields[i][j];
            ptr++;
        }
    }
    return recordSize;
}

int VirtualDisk::insertRecordToBlock(uchar *targetBlock, uchar *targetRecord, int recordSize) {
    int recordID = 0;
    int blockHeaderSize = *targetBlock;
    int availSpace = ((targetBlock[1] << 8) & 0xFF00) | targetBlock[2];
//    cout << "avail space before insertion: " << availSpace << endl;
    unsigned char *headerPointer;

    // Find empty slot to insert record
    headerPointer = targetBlock + 3;
    for (int i = 0; i < numMaxRecPerBlk; i++) {
        headerPointer = (targetBlock + 3) + (i * 5);
        if (((headerPointer[1] << 8) | headerPointer[2]) == 0) {
            recordID = i + 1;
            break;
        }
    }

    // No room to insert / Ran out of record header
    if (recordID < 1) {
        if (DEBUG_MODE) cout << "No room to insert / Ran out of record header" << endl;
        return -1;
    }
//    cout << "Inserting into record " << recordID << endl;

    int startOffset, endOffset;
    if (recordID == 1) endOffset = blkSize - 1;
    else
        endOffset = (((targetBlock[3 + ((recordID - 2) * 5) + 1] << 8) & 0xFF00) |
                     targetBlock[3 + ((recordID - 2) * 5) + 2]) - 1;
//    cout << "endOffset: " << endOffset << endl;
    startOffset = endOffset - recordSize + 1;

    // Insert Header Data
    headerPointer[1] = (startOffset >> 8) & 0xFF;
    headerPointer[2] = startOffset & 0xFF;
    headerPointer[3] = (endOffset >> 8) & 0xFF;
    headerPointer[4] = endOffset & 0xFF;

    // Insert Record Data into Block
    unsigned char *startPointer;
    startPointer = targetBlock + startOffset;
    for (int i = 0; i < recordSize; i++) {
        *(startPointer + i) = *(targetRecord + i);
    }

    // Update Block Available Space
    availSpace = availSpace - recordSize;
//    cout << "avail space after insertion: " << availSpace << endl;
    targetBlock[1] = (availSpace >> 8) & 0xFF;
    targetBlock[2] = availSpace & 0xFF;

    return recordID;
}

bool VirtualDisk::removeRecordFromBlock(uchar *targetBlock, uint recordNum) {
    uchar *pHead = targetBlock + 3 + ((recordNum - 1) * 5);
    uint startOffset = (pHead[1] << 8) | pHead[2];
    uint endOffset = (pHead[3] << 8) | pHead[4];
    uint recordSize = endOffset - startOffset + 1;

    if (startOffset == 0 && endOffset == 0) {
        cout << "Record not found in target block" << endl;
        return false;
    }

    // Remove Record
    memset(targetBlock + startOffset, 0, recordSize);
    // Remove Header and update free space count
    memset(pHead + 1, 0, 4);
    uint freeSpace = (targetBlock[1] << 8) | targetBlock[2];
    freeSpace += recordSize;
    targetBlock[1] = (freeSpace >> 8) & 0xFF;
    targetBlock[2] = freeSpace & 0xFF;
    return true;
}

vector<tuple<uchar, uchar, size_t, uchar *>> VirtualDisk::fetchRecordFromBlock(uchar *targetBlock, uint recordNum) {
    vector<tuple<uchar, uchar, size_t, uchar *>> recordSet;
    uchar *pHead = targetBlock + 3 + ((recordNum - 1) * 5);
    uint startOffset = (pHead[1] << 8) | pHead[2];
    uint endOffset = (pHead[3] << 8) | pHead[4];
    uint recordSize = endOffset - startOffset;

    if (startOffset == 0 && endOffset == 0) {
        cout << "Record not found in target block" << endl;
        return recordSet;
    }

    uchar recordBytes[recordSize];
    memcpy(recordBytes, targetBlock + startOffset, recordSize);

    uint numFields = recordBytes[0];
    uchar *pData = recordBytes + 1;
    for (int i = 0; i < numFields; i++) {
        uchar fieldID = *pData;
        pData++;
        uchar dataType = *pData;
        pData++;
        size_t dataSize = *pData;
        pData++;
        uchar *data = (uchar *) calloc(dataSize, 1);
        memcpy(data, pData, dataSize);
        recordSet.push_back(make_tuple(fieldID, dataType, dataSize, data));
        pData += dataSize;
    }
    return recordSet;
}

void VirtualDisk::printHex(unsigned char *target, size_t size, string label) {
    if (!DEBUG_MODE) return;
    cout << "Print " << label << " [" << static_cast<void *>(target) << "] :";
    for (int i = 0; i < size; ++i) {
        if (i % 20 == 0) cout << endl;
        printf("%02x ", *(target + i));
    }
    cout << endl;
}

void VirtualDisk::reportStats() {
    cout << "------------------- Virtual Disk Stats -------------------" << endl;
    cout << "| Disk Size: \t\t\t" << diskSize << " B (" << diskSize / 1000000 << " MB)" << endl;
    cout << "| Used Space: \t\t\t" << numAllocBlk * blkSize << " B" << endl;
    cout << "| Free Space: \t\t\t" << numFreeBlk * blkSize << " B" << endl;
    cout << "| % Used: \t\t\t" << fixed << setprecision(4) << (float) numAllocBlk / (float) numTotalBlk * 100 << " %"
         << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "| Total Block Count: \t\t" << numTotalBlk << endl;
    cout << "| Allocated Block Count: \t" << numAllocBlk << endl;
    cout << "| Free Block Count: \t\t" << numFreeBlk << endl;
    cout << "----------------------------------------------------------" << endl;
    cout << "| Block Size: \t\t\t" << blkSize << " B" << endl;
    cout << "| Block Header Size: \t\t" << this->blkHeaderSize << " B" << endl;
    cout << "| Max Records per Block: \t" << this->numMaxRecPerBlk << endl;
    cout << "| Physical Address Range: \t[" << (void *) (this->pDisk) << "] - [" << (void *) (this->pDisk + diskSize)
         << "]" << endl;
    cout << "----------------------------------------------------------" << endl;
}

// Destructor Method
VirtualDisk::~VirtualDisk() {
    free(pDisk);
};
