//
// Created by jingh on 25/9/2021.
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstddef>
#include <mem.h>

using namespace std;

void printHex(unsigned char *target, size_t size, string label) {
    cout << "Print " << label << ": ";
    for (int i = 0; i < size; ++i) {
        if (i % 20 == 0) cout << endl;
        printf("%02x ", *(target + i));
    }
    cout << endl;
}

void intToBytes(unsigned short int integer, unsigned char *bytes) {
    int numBytes = sizeof(integer);
    for (int i = 0; i < numBytes; i++) {
        bytes[i] = (integer >> (i * 8)) & 0x00FF;
    }
//    printf("%x\n", bytes);
//    printf("int: %x\n", numVotes);
//    cout <<"size: "<< sizeof(numVotes)<< endl;
//    printf("f3[0]: %02x\n", f3[0]);
//    printf("f3[1]: %02x\n", f3[1]);
//    printf("numVotes: %d\n", bytes[0]|(bytes[1]<<8));
//    cout << "size of f3: " << sizeof(bytes) << endl;
}

void fixedStringToBytes(string s, unsigned int length, unsigned char *bytes) {
    for (int i = 0; i < length; i++) {
        bytes[i] = s[i];
    }
}

void floatToBytes(float f, unsigned char *bytes) {
    *bytes = (unsigned int) (f * 10);
}

void packToField(unsigned char *data, unsigned char fieldID, unsigned char type, size_t dataSize,
                 unsigned char *field) {
    field[0] = fieldID;
    field[1] = type;
    field[2] = dataSize;
    for (int i = 0; i < dataSize; i++) {
        field[3 + i] = data[i];
    }
}


int packToRecord(unsigned char **fields, unsigned char numFields, unsigned char *record) {
    int recordSize = 0;
    record[0] = numFields;
    unsigned char *ptr = record;
    ptr = ptr + 1;
    recordSize++;
    for (int i = 0; i < numFields; i++) {
        int fieldSize = fields[i][2] + 3;
//        cout<<"field size: "<<fieldSize<<endl;
//        printHex(fields[i],fieldSize, "field[i]");
        recordSize = recordSize + fieldSize;
        for (int j = 0; j < fieldSize; j++) {
            *ptr = fields[i][j];
            ptr++;
        }
//        cout << endl;
    }
//    cout << "Size of record: " << recordSize << endl;
//    printHex(record,recordSize,"record");
    return recordSize;
}

void initBlock(unsigned char *targetBlock, size_t blockSize, size_t headerSize) {
    *targetBlock = headerSize;
    int offset = 1;
    int recordPointerSize = 5;
    for (int i = 0; i < headerSize; i = i + recordPointerSize) {
        *(targetBlock + i + offset) = (i / recordPointerSize) + 1;
    }
}


int insertRecord(unsigned char *targetBlock, size_t blockSize, unsigned char *targetRecord, int recordSize) {
    int recordID = 0;
    int blockHeaderSize = *targetBlock;
    unsigned char *headerPointer;
    // TODO Find empty slot to insert record

    recordID = 1;

    unsigned char *startPointer, *endPointer;
    endPointer = targetBlock + blockSize;
    startPointer = endPointer - recordSize;
    for (int i = 0; i < recordSize; i++) {
        *(startPointer + i) = *(targetRecord + i);
    }

    headerPointer = targetBlock + 1 + ((recordID-1) * 5);
    int startOffset, endOffset;
    startOffset = startPointer - targetBlock;
    endOffset = endPointer - targetBlock;
    headerPointer[1] = (startOffset >> 8) & 0xFF;
    headerPointer[2] = startOffset & 0xFF;
    headerPointer[3] = (endOffset >> 8) & 0xFF;
    headerPointer[4] = endOffset & 0xFF;


    return recordID;
}


int main2(int argc, char *argv[]) {
    char str[10] = "abcdef";
    cout << "size of str: " << sizeof(str) << endl;


    int blockSize = 100;
    int blockHeaderSize = 20;
    unsigned char *pBlk = (unsigned char *) calloc(blockSize, 1);
    cout << sizeof(*pBlk) << endl;
    if (pBlk == NULL) {
        printf("Memory not allocated.\n");
        exit(0);
    } else {
        // Memory has been successfully allocated
        printf("Memory successfully allocated using calloc.\n");

        // Get the elements of the array
        for (int i = 0; i < blockSize; ++i) {
//            pBlk[i] = i;
            pBlk[i] = 0;
        }
        initBlock(pBlk, blockSize, blockHeaderSize);

        // Print the elements of the array
//        printf("The elements of the array are:");
//        for (int i = 0; i < blockSize; ++i) {
//            if (i % 20 == 0) cout << endl;
//            printf("%02x ", pBlk[i]);
//        }
        printf("Address of block is %p\n", (void *) pBlk);
        printHex(pBlk, blockSize, "Init Block");
        cout << endl;
    }


    string tconst = "tt1234567";
    float avgRating = 7.5;
    unsigned short int numVotes = 65000;

    int dataSize = 2;
    int fieldHeaderSize = 3;
    int fieldSize = dataSize + fieldHeaderSize;
    unsigned char data1[dataSize];
    intToBytes(numVotes, data1);
//    printHex(data1, 2, "data1");
    unsigned char field1[fieldSize];
    packToField(data1, '1', 'i', 2, field1);
//    cout << "Size of field1: " << sizeof(field1) << endl;
//    printHex(field1, fieldSize, "field1");

    dataSize = 9;
    fieldSize = dataSize + fieldHeaderSize;
//    unsigned short int numVotes2 = 65000;
    unsigned char data2[dataSize];
    fixedStringToBytes(tconst, 9, data2);
    unsigned char field2[fieldSize];
    packToField(data2, '2', 's', dataSize, field2);
//    printHex(field2, fieldSize, "field2");

    dataSize = 1;
    fieldSize = dataSize + fieldHeaderSize;
//    unsigned short int numVotes3 = 65000;
    unsigned char data3[dataSize];
    floatToBytes(avgRating, data3);
    unsigned char field3[fieldSize];
    packToField(data3, '3', 'f', dataSize, field3);
//    printHex(field3, fieldSize, "field3");

    unsigned char *fields[3] = {field1, field2, field3};
    unsigned char record[sizeof(field1) + sizeof(field2) + sizeof(field3)];
    int recordSize = packToRecord(fields, 3, record);
    printHex(record, recordSize, "record 1");


    int recordID = insertRecord(pBlk, blockSize, record, recordSize);
    cout << "recordNum: " << recordID << endl;
    printHex(pBlk, blockSize, "final block");


}

