//
// Created by jingh on 2/10/2021.
//

#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include "NodeF.h"
#include <tuple>

using namespace std;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;

NodeF::NodeF(size_t blockSize) {
    this->nodeData = (char *) calloc(blockSize, 1);
    uint maxKeys = (blockSize - 13) / 12;
    setMaxSize(maxKeys);
}

NodeF::NodeF(int maxKeys) {
    uint blockSize = (maxKeys * 12) + 13;
    this->nodeData = (char *) calloc(blockSize, 1);
    setMaxSize(maxKeys);
}

bool NodeF::isLeaf() {
    return nodeData[0];
}

void NodeF::setLeaf(bool isLeaf) {
    nodeData[0] = isLeaf;
}

uint_s NodeF::getCurSize() {
    return (nodeData[1] << 8) | nodeData[2];
}

void NodeF::setCurSize(uint_s curSize) {
    nodeData[1] = curSize >> 8 & 0xFF;
    nodeData[2] = curSize & 0xFF;
}

uint_s NodeF::getMaxSize() {
    return (nodeData[3] << 8) | nodeData[4];
}

void NodeF::setMaxSize(uint_s maxSize) {
    nodeData[3] = maxSize >> 8 & 0xFF;
    nodeData[4] = maxSize & 0xFF;
}

uint NodeF::getKey(uint index) {
    if (index > getMaxSize() - 1) return NULL;
    uint numBytes = 4;
    uint offset = 5 + (index * numBytes);
    uint key = 0;
    for (int i = 0; i < numBytes; i++) {
        key = (key << 8) | nodeData[offset + i];
    }
    return key;
}

void NodeF::setKey(uint index, uint key) {
    if (index > getMaxSize() - 1) return;
    uint numBytes = 4;
    uint offset = 5 + (index * numBytes);
    for (int i = 0; i < numBytes; i++) {
        nodeData[offset + (numBytes - i - 1)] = (key >> (8 * i)) & 0xFF;
    }
}

NodeF *NodeF::getChildNode(uint index) {
    if (index > getMaxSize()) return NULL;
    uint numBytes = 8;
    uint offset = 5 + (getMaxSize() * 4) + (index * numBytes);
    unsigned long long address = 0x0;
    for (int i = 0; i < numBytes; i++) {
        address = (address << 8) | nodeData[offset + i];
    }
    return (NodeF *) address;
}

void NodeF::setChildNode(uint index, NodeF *childNode) {
    if (index > getMaxSize()) return;
    uint numBytes = 8;
    uint offset = 5 + (getMaxSize() * 4) + (index * numBytes);
    auto address = (unsigned long long) childNode;
    for (int i = 0; i < numBytes; i++) {
        nodeData[offset + (numBytes - i - 1)] = (address >> (8 * i)) & 0xFF;
    }
}