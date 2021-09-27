//
// Created by jingh on 20/9/2021.
//

#include "VirtualDisk.h"
#include <iostream>
#include <string>
#include <vector>
#include <tuple>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;

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

    this->pDisk = (uchar *) calloc(diskSize, 1);
    if (this->pDisk == nullptr) {
        cout << "Virtual Disk Creation Failed: Calloc Error" << endl;
    } else {
        for (int i = 0; i < numTotalBlk; i++) blkValidity.push_back(false);

        cout << "----- Virtual Disk Created -----" << endl;
        cout << "Disk Size: " << this->diskSize << " B" << endl;
        cout << "Block Size: " << this->blkSize << " B" << endl;
        cout << "Total no. of blocks: " << this->numTotalBlk << endl;
        cout << "Block Header Size: " << this->blkHeaderSize << " B" << endl;
        cout << "Max Records per Block: " << this->numMaxRecPerBlk << endl;
        cout << "Start Address: " << static_cast<void *>(this->pDisk) << endl;
        cout << "End Address: " << static_cast<void *>(this->pDisk + diskSize) << endl;
        cout << "--------------------------------" << endl;
    }
}

int VirtualDisk::allocBlk() {
    if (numFreeBlk < 1) {
        cout << "Block Allocation Failed: Memory Full" << endl;
        return NULL;
    } else {
        int blkOffset = 0;
        for (int i = 0; i < numTotalBlk; i++) {
            if (!blkValidity[i]) {
                blkOffset = i;
                break;
            }
        }
        uchar *pBlk = pDisk + (blkOffset * blkSize);
        cout << "Block Allocation Success. Offset: " << blkOffset << " Physical Address: " << static_cast<void *>(pBlk)
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
        cout << "Block De-allocation Failed: No Allocated Blocks" << endl;
        return false;
    } else if (!blkValidity[blkOffset]) {
        cout << "Block De-allocation Failed: Block not allocated" << endl;
        return false;
    } else {
        cout << "Block De-allocation Success" << endl;
        blkValidity[blkOffset] = false;
        return true;
    }
}


tuple<uchar *, uint> VirtualDisk::addRecord(vector<tuple<uchar, uchar, size_t>> dataFormat, vector<string> data) {
    cout << "Add Record" << endl;
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
        cout << "No allocated blocks." << endl;
        blkOffset = allocBlk(); // No allocated blocks
    } else {
        for (int i = 0; i < numTotalBlk; i++) {
            int availSpace = (*(pDisk + (i * blkSize) + 1) << 8) | (*(pDisk + (i * blkSize) + 2));
            bool isHeaderFull = ((*(pDisk + (i * blkSize) + ((numMaxRecPerBlk - 1) * 5) + 4) << 8) |
                                 (*(pDisk + (i * blkSize) + ((numMaxRecPerBlk - 1) * 5) + 5))) != 0;
            if (blkValidity[i] && availSpace >= recordSize && !isHeaderFull) { // Found an allocated block with sufficient space
                blkOffset = i;
                cout << "Block " << i << " has sufficient space." << endl;
                break;
            }
        }
        if (blkOffset == -1) {
            cout << "No space in all allocated blocks." << endl;
            blkOffset = allocBlk(); // No space in all allocated blocks
        }
    }

    // Add Record to Block
    uchar *pBlk = pDisk + (blkOffset * blkSize);
    int recordID = insertRecordToBlock(pBlk, blkSize, record, recordSize);
    if (recordID == -1) {
        blkOffset = allocBlk();
        pBlk = pDisk + (blkOffset * blkSize);
        recordID = insertRecordToBlock(pBlk, blkSize, record, recordSize);
    }
    cout << "recordID: " << recordID << endl;

    printHex(pBlk, blkSize, "After add");
    return make_tuple(nullptr, 0);
}

bool VirtualDisk::deleteRecord(tuple<uchar *, uint> recordDirectory) {
    cout << "Delete Record" << endl;
    return false;
}

bool VirtualDisk::fetchRecord(tuple<uchar *, uint> recordDirectory) {
    return false;
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

int VirtualDisk::insertRecordToBlock(uchar *targetBlock, size_t blockSize, uchar *targetRecord, int recordSize) {
    int recordID = 0;
    int blockHeaderSize = *targetBlock;
    int availSpace = ((targetBlock[1] << 8) & 0xFF00) | targetBlock[2];
//    cout << "avail space before insertion: " << availSpace << endl;
    unsigned char *headerPointer;
    // TODO Find empty slot to insert record
    headerPointer = targetBlock + 3;
    for (int i = 0; i < numMaxRecPerBlk; i++) {
        headerPointer = (targetBlock + 3) + (i * 5);
//        printf("%x\n",headerPointer[2]);
//        printf("%x\n",((headerPointer[1] << 8) & 0xFF00) | headerPointer[2]);
        if (((headerPointer[1] << 8) | headerPointer[2]) == 0) {
            recordID = i + 1;
            break;
        }
    }

    // No room to insert / Ran out of record header
    if (recordID < 1) {
        cout << "No room to insert / Ran out of record header" << endl;
        return -1;
    }
//    cout << "Inserting into record " << recordID << endl;

    int startOffset, endOffset;
    if (recordID == 1) endOffset = blockSize - 1;
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

void VirtualDisk::printHex(unsigned char *target, size_t size, string label) {
    cout << "Print " << label << " [" << static_cast<void *>(target) << "] :";
    for (int i = 0; i < size; ++i) {
        if (i % 20 == 0) cout << endl;
        printf("%02x ", *(target + i));
    }
    cout << endl;
}

void VirtualDisk::reportStats() {
    cout << "------- Database Stats --------" << endl;
    cout << "Total Blocks: \t\t" << numTotalBlk << endl;
    cout << "Allocated Blocks: \t" << numAllocBlk << endl;
    cout << "Free Blocks: \t\t" << numFreeBlk << endl;
    cout << "-------------------------------" << endl;
}

// Destructor Method
VirtualDisk::~VirtualDisk() = default;
