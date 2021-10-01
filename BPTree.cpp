#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include "BPTree.h"
#include "Node.h"
#include <tuple>
#include <queue>

using namespace std;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short int uint_s;

const bool DEBUG_MODE = false;

BPTree::BPTree(int nodeSize) {
    this->nodeSize = nodeSize;
    this->rootNode = nullptr;
}

void BPTree::insertKey(int newKey, tuple<uint, void *, uint_s> *keyPtr) {
    //    cout<< (void*)keyPtr<<endl;
    if (rootNode == nullptr) {
        rootNode = new Node(nodeSize);
        rootNode->leaf = true;
        rootNode->key[0] = newKey;
        rootNode->curSize = 1;
        rootNode->childNode[0] = (Node *) keyPtr;
        if (DEBUG_MODE)cout << "New Tree" << endl;
    } else //root node not null
    {
        Node *ptrNode = rootNode;
        Node *parent;
        int childPtr;

        while (ptrNode->leaf == false) //find the leaf node to insert new key
        {
            parent = ptrNode;
            for (int i = 0; i < ptrNode->curSize; i++) {
                childPtr = i;
                if (newKey < ptrNode->key[i]) //if newkey smaller than current key, traverse to ith child node
                {
                    ptrNode = ptrNode->childNode[i];
                    break;
                }
                if (i == ptrNode->curSize - 1) //if reached last key, traverse to most right node
                {
                    ptrNode = ptrNode->childNode[i + 1];
                    break;
                }
            }
        }

        if (ptrNode->curSize < nodeSize) //if leaf node still has space
        {
            if (DEBUG_MODE)cout << "leaf node still has space" << endl;

            int keyPointer = 0;
            for (int i = 0; i < ptrNode->curSize; i++) { //find position to insert key
                if (newKey < ptrNode->key[i]) break;
                else keyPointer++;
            }

//            cout << "keyPointer: " << keyPointer << endl;
//            cout << "ptrNode->curSize: " << ptrNode->curSize << endl;

            for (int j = ptrNode->curSize; j > keyPointer; j--) { //insert new key
                //move all keys back by 1
                ptrNode->key[j] = ptrNode->key[j - 1];
                ptrNode->childNode[j] = ptrNode->childNode[j - 1];
            }
            ptrNode->key[keyPointer] = newKey;
            ptrNode->childNode[keyPointer] = (Node *) keyPtr;
            ptrNode->curSize++;

        } else //if leaf node no space, make new node
        {
            if (DEBUG_MODE)cout << "leaf node no space, make new node" << endl;
            if (DEBUG_MODE)printNode(ptrNode, "orig leaf node");
            Node *newLeaf = new Node(nodeSize);
            newLeaf->leaf = true;
            Node *tempNode = new Node(nodeSize + 1);
            tempNode->curSize = 0;

            int keyCursor = 0;
            for (int i = 0; i < ptrNode->curSize; i++) //find position to input new key
            {
                if (newKey < ptrNode->key[i])break;
                else keyCursor++;
            }
//            cout << "keyCursor: " << keyCursor << endl;

            int j = 0;
            for (int i = 0; i < nodeSize + 1; i++) //copy all keys and insert new key into tempNode
            {
//                cout << "i,j: " << i << " " << j << endl;
                if (keyCursor != i) {
                    tempNode->key[i] = ptrNode->key[j];
                    tempNode->childNode[i] = ptrNode->childNode[j];
//                    cout << "add " << ptrNode->key[j] << endl;
                    j++;
                } else {
                    tempNode->key[i] = newKey;
                    tempNode->childNode[i] = (Node *) keyPtr;
//                    cout << "add new " << newKey << endl;
                }
                tempNode->curSize++;
            }
            tempNode->childNode[nodeSize + 1] = ptrNode->childNode[nodeSize];

//            printNode(tempNode, "temp leaf node");

            ptrNode->curSize = ceil((nodeSize + 1) / 2);
            newLeaf->curSize = floor((nodeSize + 1) / 2);

//            cout << "ptrNode->curSize: " << ptrNode->curSize << endl;
//            cout << "newLeaf->curSize: " << newLeaf->curSize << endl;

            for (int i = 0; i < nodeSize; i++) //copy from temp node to initial node, clearing excess keys
            {
                if (i < ptrNode->curSize) {
                    ptrNode->key[i] = tempNode->key[i];
                    ptrNode->childNode[i] = tempNode->childNode[i];
                } else {
                    ptrNode->key[i] = NULL;
                    ptrNode->childNode[i] = nullptr;
                }
            }
            for (int i = 0; i < newLeaf->curSize; i++) //copy from temp node to new node
            {
                newLeaf->key[i] = tempNode->key[i + ptrNode->curSize];
                newLeaf->childNode[i] = tempNode->childNode[i + ptrNode->curSize];
            }

            ptrNode->childNode[nodeSize] = newLeaf; //set last node ptr to point to next leaf node
            newLeaf->childNode[nodeSize] = tempNode->childNode[nodeSize + 1];

//            printNode(ptrNode, "orig leaf node");
//            printNode(newLeaf, "new leaf node");

            if (ptrNode == rootNode) {
                Node *newRoot = new Node(nodeSize);
                newRoot->leaf = false;
                newRoot->key[0] = newLeaf->key[0]; //key of new root is smallest key on right node
                newRoot->curSize = 1;
                newRoot->childNode[0] = ptrNode; //set child pointers
                newRoot->childNode[1] = newLeaf;
                rootNode = newRoot;
            } else {
                insertInternal(newLeaf->key[0], parent, newLeaf);
            }
        }
    }
}

int BPTree::heightOfTree(Node *cursor) //initial node should be root
{
    if (cursor == NULL)
        return 0;
    if (cursor->leaf == true) {
        return 1;
    } else {
        return heightOfTree(cursor->childNode[0]) + 1;
    }
}

void BPTree::insertInternal(int newKey, Node *ptrNode, Node *child) {
    if (DEBUG_MODE)cout << "insert internal" << endl;
    if (DEBUG_MODE)printNode(ptrNode, "orig internal node");
    if (ptrNode->curSize < nodeSize) //if internal node still has space
    {
        if (DEBUG_MODE)cout << "internal node still has space" << endl;

        int keyPointer = 0;
        for (int i = 0; i < ptrNode->curSize; i++) { //find position to insert key
            if (newKey < ptrNode->key[i]) break;
            else keyPointer++;
        }

//        cout << "keyPointer: " << keyPointer << endl;
//        cout << "ptrNode->curSize: " << ptrNode->curSize << endl;
//        cout << "newkey: " << newKey << endl;
//        cout << "child: " << child << endl;

        for (int j = ptrNode->curSize; j > keyPointer; j--) { //insert new key
            //move all keys back by 1
            ptrNode->key[j] = ptrNode->key[j - 1];
            ptrNode->childNode[j + 1] = ptrNode->childNode[j];
        }
        ptrNode->key[keyPointer] = newKey;
        ptrNode->childNode[keyPointer + 1] = child;
        ptrNode->curSize++;

        if (DEBUG_MODE)printNode(ptrNode, "new internal node");

    } else //if internal node no space, make new node
    {
        if (DEBUG_MODE)cout << "internal node no space, make new node" << endl;
        Node *newNode = new Node(nodeSize);
        newNode->leaf = false;
        Node *tempNode = new Node(nodeSize + 1);
        tempNode->curSize = 0;

        int keyCursor = 0;
        for (int i = 0; i < ptrNode->curSize + 1; i++) { //find position to input new key
//            cout << "key: " << getMinKey(ptrNode->childNode[i]) << endl;
            if (newKey < getMinKey(ptrNode->childNode[i])) break;
            else keyCursor++;
        }

//        cout << "keyCursor: " << keyCursor << endl;
//        cout << "newkey: " << newKey << endl;
//        cout << "child: " << child << endl;

        int j = 0;
        for (int i = 0; i < nodeSize + 2; i++) //copy all keys and insert new key into tempNode
        {
//            cout << "i: " << i << endl;
            if (keyCursor != i) {
                if (i != 0)tempNode->key[i - 1] = getMinKey(ptrNode->childNode[j]);
                tempNode->childNode[i] = ptrNode->childNode[j];
//                cout << "insert " << ptrNode->key[j] << endl;
                j++;
            } else {
                if (i != 0) tempNode->key[i - 1] = newKey;
                tempNode->childNode[i] = child;
//                cout << "insert new " << newKey << endl;
            }
            if (i != nodeSize + 1) tempNode->curSize++;
        }

//        cout << ptrNode->key[0];
//        printNode(tempNode, "temp node");

        ptrNode->curSize = ceil((float) nodeSize / 2);
        newNode->curSize = floor((float) nodeSize / 2);
        //ptrNode->childNode[nodeSize] = newLeaf; //set last node ptr to point to next leaf node
        //newLeaf->childNode[nodeSize] = ptrNode->childNode[nodeSize];

//        cout << "Cur node size: " << ptrNode->curSize << endl;
//        cout << "New node size:" << newLeaf->curSize << endl;
        for (int i = 0; i < nodeSize; i++) //copy from temp node to initial node, clearing excess keys
        {
            if (i < ptrNode->curSize + 1) {
                if (i < ptrNode->curSize) ptrNode->key[i] = tempNode->key[i];
                ptrNode->childNode[i] = tempNode->childNode[i];
            } else {
                if (i < ptrNode->curSize) ptrNode->key[i] = NULL;
                ptrNode->childNode[i] = nullptr;
            }
        }
        for (int i = 0; i < newNode->curSize + 1; i++) //copy from temp node to new node
        {
            if (i < newNode->curSize)
                newNode->key[i] = tempNode->key[i + ptrNode->curSize + 1];
            newNode->childNode[i] = tempNode->childNode[i + ptrNode->curSize + 1];
        }

//        printNode(ptrNode, "ptr internal node");
//        printNode(newNode, "new internal node");

        if (ptrNode == rootNode) {
            Node *newRoot = new Node(nodeSize);
            newRoot->leaf = false;
            newRoot->key[0] = newNode->key[0]; //key of new root is smallest key on right node
            newRoot->curSize = 1;
            newRoot->childNode[0] = ptrNode; //set child pointers
            newRoot->childNode[1] = newNode;
            rootNode = newRoot;
        } else {
            Node *parent = findParent(rootNode, ptrNode);
//            printTree(parent);
            insertInternal(getMinKey(newNode), parent, newNode);
        }
    }
}

Node *BPTree::searchForNode(int key) {
    if (rootNode == NULL) {
        return nullptr;
    } else {
        Node *ptrNode = rootNode;
        int pointer = 0;
        while (ptrNode->leaf == false) //find the leaf node with delete key
        {

            for (int i = 0; i < ptrNode->curSize; i++) {
                if (key > ptrNode->key[i]) {
                    pointer = i + 1;
                    break;
                }
            }
            ptrNode = ptrNode->childNode[pointer];
        }
        return ptrNode;
    }
}

vector<tuple<uint, void *, uint_s>> BPTree::searchForRange(int start, int end) {
    Node *searchNode;
    searchNode = searchForNode(start);
    if (searchNode == nullptr)
        return {};
    vector<tuple<uint, void *, uint_s>> rangeOfRecords;
    int startKeyPos;
    for (int i = 0; i < searchNode->curSize; i++) //find position of first key >= start in node
    {
        if (start <= searchNode->key[i]) {
            startKeyPos = i;
            break;
        }
    }
    int cursorKey = startKeyPos;
    while (searchNode->key[cursorKey] <= end) //find all keys in range
    {
        tuple<uint, void *, uint_s> *keyPtr = (tuple<uint, void *, uint_s> *) searchNode->childNode[cursorKey];
        rangeOfRecords.push_back(*keyPtr);
        cursorKey++;
        if (cursorKey == searchNode->curSize - 1) //if reach last key in node, jump to next node
        {
            searchNode = searchNode->childNode[nodeSize];
            cursorKey = 0;
        }
    }
    return rangeOfRecords;
}

Node *BPTree::findParent(Node *ptrNode, Node *child) {
    Node *parent;
    if (ptrNode->leaf || (ptrNode->childNode[0])->leaf) //stop recurssion when hit last 2 levels
    {
        return NULL;
    }
    for (int i = 0; i < ptrNode->curSize + 1; i++) //iterate through all pointers in node
    {
        if (ptrNode->childNode[i] == child) {
            parent = ptrNode;
            return parent;
        } else // if not parent, call findParent recursively
        {
            parent = findParent(ptrNode->childNode[i], child);
            if (parent != NULL)
                return parent;
        }
    }
    return parent;
}

void BPTree::deleteKey(int deleteKey) {
    if (rootNode == NULL) {
        return;
    } else {
        Node *ptrNode = rootNode;
        Node *parent;
        int leftSibling = 0, rightSibling = 0;
        int pointer = 0;
        while (ptrNode->leaf == false) //find the leaf node with delete key
        {

            for (int i = 0; i < ptrNode->curSize; i++) {
                if (deleteKey > ptrNode->key[i]) {
                    pointer = i + 1;
                    leftSibling = pointer - 1;
                    rightSibling = pointer + 1;
                    break;
                }
                parent = ptrNode;
                ptrNode = ptrNode->childNode[pointer];
            }
            int keyPointer = 0;
            bool keyExist = false;
            for (int i = 0; i < ptrNode->curSize; i++) //find key in leaf node
            {
                if (deleteKey == ptrNode->key[i]) {
                    keyPointer = i;
                    keyExist = true;
                    break;
                }
            }
            if (keyExist) {
                ptrNode->key[keyPointer] = NULL; //delete key
                ptrNode->curSize = ptrNode->curSize - 1;
                ptrNode->childNode[keyPointer] = nullptr;

                // CASE 1
                if (ptrNode->curSize > floor((nodeSize + 1) / 2)) //if enough keys in node after deleting
                {
                    for (int i = keyPointer; i < nodeSize; i++) //squeeze keys in node
                    {
                        ptrNode->key[i] = ptrNode->key[i + 1];
                    }

                    if (pointer > 0 && keyPointer == 0) {
                        //parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
                        // TODO update tree
                        updateTreeAftDelete(deleteKey, ptrNode->key[0]);
                    }
                    return;
                }
                    //CASE 2 borrow from sibling node
                else {
                    Node *rightSiblingNode = nullptr;
                    Node *leftSiblingNode = nullptr;
                    bool cannotShare = false;
                    if (leftSibling >= 0) //if left sibling exist
                        leftSiblingNode = parent->childNode[leftSibling];
                    if (rightSibling < nodeSize + 2) //if right sibling exist
                        rightSiblingNode = parent->childNode[rightSibling];

                    if (leftSiblingNode != nullptr) {
                        if (leftSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) //if left sibling can share keys
                        {
                            int shareKey = leftSiblingNode->key[leftSiblingNode->curSize - 1];
                            for (int i = 0;
                                 i < ptrNode->curSize; i++) //squeeze keys in delete node and leave first key empty
                            {
                                if (ptrNode->key[i] == NULL) {
                                    if (i == 0)
                                        break;
                                    else {
                                        for (int j = i; j > 0; j--) {
                                            ptrNode->key[j] = ptrNode->key[j - 1];
                                            ptrNode->childNode[j] = ptrNode->childNode[j - 1];
                                        }
                                        break;
                                    }
                                }
                            }
                            //insert shared key in delete node
                            ptrNode->key[0] = shareKey;
                            ptrNode->childNode[0] = leftSiblingNode->childNode[leftSiblingNode->curSize - 1];
                            ptrNode->curSize++;
                            //delete shared key from sibling
                            leftSiblingNode->key[leftSiblingNode->curSize - 1] = NULL;
                            leftSiblingNode->childNode[leftSiblingNode->curSize - 1] = nullptr;
                            leftSiblingNode->curSize--;

                            //TODO update tree function
                            updateTreeAftDelete(deleteKey, ptrNode->key[0]);
                            return;
                        } else {
                            cannotShare = true;
                        }
                    } else if (rightSiblingNode != NULL) //if right sibling can share keys
                    {
                        if (rightSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) {
                            int shareKey = rightSiblingNode->key[0];
                            for (int i = 0;
                                 i < ptrNode->curSize; i++) //squeeze keys in delete node and leave last key empty
                            {

                                if (ptrNode->key[i] == NULL) {
                                    for (int j = i; j < nodeSize + 1; j--) {
                                        ptrNode->key[j] = ptrNode->key[j + 1];
                                        ptrNode->childNode[j] = ptrNode->childNode[j + 1];
                                    }
                                    break;
                                }
                            }
                            //insert shared key in delete node
                            ptrNode->key[ptrNode->curSize] = shareKey;
                            ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[0];
                            ptrNode->curSize++;
                            //delete shared key from sibling
                            for (int i = 0; i < rightSiblingNode->curSize; i++) {
                                rightSiblingNode->key[i] = rightSiblingNode->key[i + 1];
                                rightSiblingNode->childNode[i] = rightSiblingNode->childNode[i + 1];
                            }

                            //TODO update tree function
                            updateTreeAftDelete(deleteKey, rightSiblingNode->key[0]);
                            return;
                        } else {
                            cannotShare = true;
                        }
                    }
                    if (cannotShare) //both left and right siblings unable to share keys
                    {
                        for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
                        {
                            ptrNode->key[i] = ptrNode->key[i + 1];
                        }
                        if (leftSiblingNode != NULL) {
                            for (int i = 0; i < ptrNode->curSize; i++) {
                                //transfer all keys into left sibling
                                leftSiblingNode->key[leftSiblingNode->curSize + i] = ptrNode->key[i];
                                leftSiblingNode->childNode[leftSiblingNode->curSize + i] = ptrNode->childNode[i];
                            }
                            //shift last ptr over to left sibling node
                            leftSiblingNode->childNode[nodeSize + 1] = ptrNode->childNode[nodeSize + 1];
                            leftSiblingNode->curSize += ptrNode->curSize;
                            delete ptrNode;
                            deleteInternal(parent->key[leftSibling], parent, leftSiblingNode);
                            return;
                        } else if (rightSiblingNode != NULL) {
                            for (int i = 0; i < rightSiblingNode->curSize; i++) {
                                //transfer all keys from right sibling
                                ptrNode->key[ptrNode->curSize] = rightSiblingNode->key[i];
                                ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[i];
                            }
                            //shift last ptr over from right sibling node
                            ptrNode->childNode[nodeSize + 1] = rightSiblingNode->childNode[nodeSize + 1];
                            ptrNode->curSize += rightSiblingNode->curSize;
                            delete rightSiblingNode;
                            deleteInternal(parent->key[rightSibling - 1], parent, ptrNode);
                        }
                    }
                }
            }
        }
    }
}

void BPTree::deleteInternal(int deleteKey, Node *ptrNode, Node *child) {
    if (ptrNode == rootNode) {
        if (ptrNode->curSize == 1) {
            rootNode = child;
            delete ptrNode;
            return;
        }
    } else {
        int keyPointer;
        for (int i = 0; i < ptrNode->curSize; i++) //find key in internal node
        {
            if (deleteKey == ptrNode->key[i]) {
                keyPointer = i;
                break;
            }
        }
        ptrNode->key[keyPointer] = NULL; //delete key
        ptrNode->curSize = ptrNode->curSize - 1;
        ptrNode->childNode[keyPointer] = nullptr;
        int leftSibling = 0, rightSibling = 0, pointer = 0;
        Node *parent = findParent(rootNode, ptrNode);

        // CASE 1
        if (ptrNode->curSize > floor((nodeSize + 1) / 2)) //if enough keys in node after deleting
        {
            for (int i = keyPointer; i < nodeSize; i++) //squeeze keys in node
            {
                ptrNode->key[i] = ptrNode->key[i + 1];
            }

            if (pointer > 0 && keyPointer == 0) {
                //parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
                // TODO update tree
                updateTreeAftDelete(deleteKey, ptrNode->key[0]);
            }
            return;
        }
            //CASE 2 borrow from sibling node
        else {
            Node *rightSiblingNode = nullptr;
            Node *leftSiblingNode = nullptr;
            bool cannotShare = false;
            if (leftSibling >= 0) //if left sibling exist
                leftSiblingNode = parent->childNode[leftSibling];
            if (rightSibling < nodeSize + 2) //if right sibling exist
                rightSiblingNode = parent->childNode[rightSibling];

            if (leftSiblingNode != nullptr) {
                if (leftSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) //if left sibling can share keys
                {
                    int shareKey = leftSiblingNode->key[leftSiblingNode->curSize - 1];
                    for (int i = 0;
                         i < ptrNode->curSize; i++) //squeeze keys in delete node and leave first key empty
                    {
                        if (ptrNode->key[i] == NULL) {
                            if (i == 0)
                                break;
                            else {
                                for (int j = i; j > 0; j--) {
                                    ptrNode->key[j] = ptrNode->key[j - 1];
                                    ptrNode->childNode[j] = ptrNode->childNode[j - 1];
                                }
                                break;
                            }
                        }
                    }
                    //insert shared key in delete node
                    ptrNode->key[0] = shareKey;
                    ptrNode->childNode[0] = leftSiblingNode->childNode[leftSiblingNode->curSize - 1];
                    ptrNode->curSize++;
                    //delete shared key from sibling
                    leftSiblingNode->key[leftSiblingNode->curSize - 1] = NULL;
                    leftSiblingNode->childNode[leftSiblingNode->curSize - 1] = nullptr;
                    leftSiblingNode->curSize--;

                    //TODO update tree function
                    updateTreeAftDelete(deleteKey, ptrNode->key[0]);
                    return;
                } else {
                    cannotShare = true;
                }
            } else if (rightSiblingNode != NULL) //if right sibling can share keys
            {
                if (rightSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) {
                    int shareKey = rightSiblingNode->key[0];
                    for (int i = 0;
                         i < ptrNode->curSize; i++) //squeeze keys in delete node and leave last key empty
                    {

                        if (ptrNode->key[i] == NULL) {
                            for (int j = i; j < nodeSize + 1; j--) {
                                ptrNode->key[j] = ptrNode->key[j + 1];
                                ptrNode->childNode[j] = ptrNode->childNode[j + 1];
                            }
                            break;
                        }
                    }
                    //insert shared key in delete node
                    ptrNode->key[ptrNode->curSize] = shareKey;
                    ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[0];
                    ptrNode->curSize++;
                    //delete shared key from sibling
                    for (int i = 0; i < rightSiblingNode->curSize; i++) {
                        rightSiblingNode->key[i] = rightSiblingNode->key[i + 1];
                        rightSiblingNode->childNode[i] = rightSiblingNode->childNode[i + 1];
                    }

                    //TODO update tree function
                    updateTreeAftDelete(deleteKey, rightSiblingNode->key[0]);
                    return;
                } else {
                    cannotShare = true;
                }
            }
            if (cannotShare) //both left and right siblings unable to share keys
            {
                for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
                {
                    ptrNode->key[i] = ptrNode->key[i + 1];
                }
                if (leftSiblingNode != NULL) {
                    for (int i = 0; i < ptrNode->curSize; i++) {
                        //transfer all keys into left sibling
                        leftSiblingNode->key[leftSiblingNode->curSize + i] = ptrNode->key[i];
                        leftSiblingNode->childNode[leftSiblingNode->curSize + i] = ptrNode->childNode[i];
                    }
                    //shift last ptr over to left sibling node
                    leftSiblingNode->childNode[nodeSize + 1] = ptrNode->childNode[nodeSize + 1];
                    leftSiblingNode->curSize += ptrNode->curSize;
                    delete ptrNode;
                    deleteInternal(parent->key[leftSibling], parent, leftSiblingNode);
                    return;
                } else if (rightSiblingNode != NULL) {
                    for (int i = 0; i < rightSiblingNode->curSize; i++) {
                        //transfer all keys from right sibling
                        ptrNode->key[ptrNode->curSize] = rightSiblingNode->key[i];
                        ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[i];
                    }
                    //shift last ptr over from right sibling node
                    ptrNode->childNode[nodeSize + 1] = rightSiblingNode->childNode[nodeSize + 1];
                    ptrNode->curSize += rightSiblingNode->curSize;
                    delete rightSiblingNode;
                    deleteInternal(parent->key[rightSibling - 1], parent, ptrNode);
                }
            }
        }
    }
}

void BPTree::updateTreeAftDelete(int deleteKey, int newKey) {
    Node *ptrNode = rootNode;
    bool keyFound = false;
    int pointer = 0;
    while (!keyFound) {
        for (int i = 0; i < ptrNode->curSize; i++) //search within the node
        {
            if (ptrNode->key[i] == deleteKey) //if key found
            {
                ptrNode->key[i] = newKey;
                keyFound = true;
                break;
            } else if (deleteKey < ptrNode->key[i]) //find which child pointer to go
            {
                if (i = 0) //delete key smaller than first key
                {
                    pointer = 0;
                } else {
                    pointer = i;
                }
                break;
            }
        }
        ptrNode = ptrNode->childNode[pointer];
    }
}

int BPTree::getMinKey(Node *ptrNode) {
    if (ptrNode == nullptr) return NULL;
    while (!ptrNode->leaf) ptrNode = ptrNode->childNode[0];
    return ptrNode->key[0];
}

void BPTree::printTree(Node *root) {
    bool printAddress = false;
    uint numNodes = 0;
    queue<Node *> printQueue;

    cout << "+++++++++++++++++ Printing B+Tree +++++++++++++++++" << endl;
    if (root != nullptr) {
        printQueue.push(root);
        printQueue.push(nullptr);
    } else {
        cout << "Nothing to print." << endl;
        return;
    }
    while (!printQueue.empty()) {
        Node *curNode = printQueue.front();
        printQueue.pop();

        if (curNode == nullptr) {
            if (printQueue.size() > 1) {
//                cout << " \\n ";
                cout << endl << "  \\" << endl;
            }
        } else {
            numNodes++;
            if (printAddress) cout << curNode << " ";
            if (printAddress) cout << "[" << (curNode->leaf ? "L" : "N") << curNode->curSize << " ";
            if (!printAddress) cout << "[";
            int i;
            for (i = 0; i < curNode->curSize; i++) {
                if (printAddress) cout << curNode->childNode[i] << " ";
                if (printAddress) cout << curNode->key[i] << " ";
                if (!printAddress) cout << curNode->key[i] << ((i < curNode->curSize - 1) ? " " : "");
                if (!curNode->leaf && curNode->childNode[i] != nullptr) {
                    printQueue.push(curNode->childNode[i]);
                }
            }
            if (curNode->leaf) {
                if (printAddress) cout << curNode->childNode[curNode->maxSize];
            } else {
                if (printAddress) cout << curNode->childNode[i];
                printQueue.push(curNode->childNode[i]);
            }
            cout << "] ";
            if (printQueue.front() == nullptr && printQueue.size() > 1) {
                //                cout<<" (NL) ";
                printQueue.push(nullptr);
            }
        }
    }
    cout << "\n++++++++++++++++++ No. of Nodes: " << numNodes << " ++++++++++++++++" << endl
         << endl;
}

void BPTree::printNode(Node *node, string label) {
    if (node != nullptr) {
        cout << "Print " << label << ": " << node << "[" << (node->leaf ? "L" : "N") << node->curSize << "/"
             << node->maxSize << " ";
        int i;
        for (i = 0; i < node->curSize; i++) {
            cout << node->childNode[i] << " ";
            cout << node->key[i] << " ";
        }
        if (node->leaf)cout << node->childNode[node->maxSize];
        else cout << node->childNode[i];
        cout << "] " << endl;
    } else {
        cout << "Nothing to print." << endl;
        return;
    }
}
