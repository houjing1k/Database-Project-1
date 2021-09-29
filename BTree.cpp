#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
using namespace std;

const int nodeSize=3;

class Node 
{
	bool leaf;
	int *key;
	int curSize;
	int maxSize;
	
	Node **childNode; //ptr to next node
	friend class BTree;

	public:
		Node()
		{
			maxSize = nodeSize;
		}

};

class BTree
{
	Node* rootNode;
	void insertInternal(int newKey, Node* ptrNode, Node* child);
	Node* findParent(Node*, Node*);
	public:
	void insertKey(int);
	int heightOfTree(Node* cursor);
	void deleteKey(int);
	void deleteInternal(int deleteKey, Node* ptrNode, Node* child);
	

	BTree()
	{
		rootNode = NULL;
	}
};

Node::Node() {
	key = new int[maxSize];
	childNode = new Node * [maxSize + 1];
}

void BTree::insertKey(int newKey)
{
	if (rootNode == NULL)
	{
		rootNode = new Node;	
		rootNode->leaf = true;
		rootNode->key[0] = newKey;
		rootNode->curSize = 1;
	}
	else //root node not null
	{
		Node* ptrNode = rootNode;
		Node* parent;

		while (ptrNode->leaf == false) //find the leaf node to insert new key
		{
			parent = ptrNode;
			for (int i = 0; i < ptrNode->curSize; i++) 
			{
				if (newKey < ptrNode->key[i]) //if newkey smaller than current key, traverse to ith child node
				{
					ptrNode = ptrNode->childNode[i];
					break;
				}
				if (i == ptrNode->curSize -1) //if reached last key, traverse to most right node
				{
					ptrNode = ptrNode->childNode[i + 1];
					break;
				}
			}
		}

		if (ptrNode->curSize < nodeSize) //if leaf node still has space
		{
			for (int i = 0;i < ptrNode->curSize;i++)
			{
				if (newKey < ptrNode->key[i])//find position to insert key
				{
					for (int j = ptrNode->curSize+1;j > i;j--)
					{
						//move all keys back by 1
						ptrNode->key[j] = ptrNode->key[j - 1];
					}
					ptrNode->key[i] = newKey;
					ptrNode->curSize++;
					ptrNode->childNode[ptrNode->curSize] = ptrNode->childNode[ptrNode->curSize - 1];
					ptrNode->childNode[ptrNode->curSize - 1] = NULL;
					break;
				}
			}
		}
		else //if leaf node no space, make new node
		{
			Node* newLeaf = new Node;
			newLeaf->leaf = true;
			int keyArray[nodeSize+1];
			int keyCursor;
			for (int i = 0;i < nodeSize;i++)//store all keys in array and find position of new key
			{
				keyArray[i] = ptrNode->key[i];
				if (newKey > ptrNode->key[i])
					keyCursor = i;
			}
			for (int j = nodeSize + 1;j > keyCursor;j--)// shift all keys > new key to make space
			{
				keyArray[j] = keyArray[j - 1];
			}
			keyArray[keyCursor] = newKey; //insert new key into key array
			ptrNode->curSize = (nodeSize + 1) / 2;
			newLeaf->curSize = nodeSize + 1 - (nodeSize+ 1) / 2;
			ptrNode->childNode[ptrNode->curSize] = newLeaf; //set current node ptr to point to next leaf node
			newLeaf->childNode[newLeaf->curSize] = ptrNode->childNode[nodeSize];
			ptrNode->childNode[nodeSize] = NULL;

			for (int i = 0; i < ptrNode->curSize; i++) 
			{
				ptrNode->key[i] = keyArray[i];
			}
			for (int i = 0; i < newLeaf->curSize; i++) 
			{
				newLeaf->key[i] = keyArray[ptrNode->curSize+i];
			}
			if (ptrNode == rootNode) 
			{
				Node* newRoot = new Node;
				newRoot->leaf = false;
				newRoot->key[0] = newLeaf->key[0]; //key of new root is smallest key on right node
				newRoot->curSize = 1;
				newRoot->childNode[0] = ptrNode; //set child pointers
				newRoot->childNode[1] = newLeaf;
				rootNode = newRoot;
			}
			else {
				insertInternal(newLeaf->key[0], parent, newLeaf);
			}
		}

	}
}

int BTree::heightOfTree(Node* cursor)//initial node should be root
{
	if (cursor == NULL)
		return;
	if (cursor->leaf == true)
	{
		return 1;
	}
	else
	{
		return heightOfTree(cursor->childNode[0]) + 1;
	}
	
}

void BTree::insertInternal(int newKey, Node* ptrNode, Node* child)
{
	if (ptrNode->curSize < nodeSize) //if leaf node still has space
	{
		for (int i = 0;i < ptrNode->curSize;i++)
		{
			if (newKey < ptrNode->key[i])//find position to insert key
			{
				for (int j = ptrNode->curSize + 1;j > i;j--)
				{
					//move all keys back by 1
					ptrNode->key[j] = ptrNode->key[j - 1];
				}
				ptrNode->key[i] = newKey;
				ptrNode->curSize++;
				ptrNode->childNode[i + 1] = child;
				break;
			}
		}
	}
	else //if leaf node no space, make new node
	{
		Node* newInternal = new Node;
		newInternal->leaf = false;
		int keyArray[nodeSize + 1];
		int keyCursor;
		for (int i = 0;i < nodeSize;i++)//store all keys in array and find position of new key
		{
			keyArray[i] = ptrNode->key[i];
			if (newKey > ptrNode->key[i])
				keyCursor = i;
		}
		for (int j = nodeSize + 1;j > keyCursor;j--)// shift all keys > new key to make space
		{
			keyArray[j] = keyArray[j - 1];
		}
		keyArray[keyCursor] = newKey; //insert new key into key array
		ptrNode->curSize = (nodeSize + 1) / 2;
		newInternal->curSize = nodeSize + 1 - (nodeSize + 1) / 2;
		ptrNode->childNode[ptrNode->curSize] = newInternal; //set current node ptr to point to next leaf node
		newInternal->childNode[newInternal->curSize] = ptrNode->childNode[nodeSize];
		ptrNode->childNode[nodeSize] = NULL;

		for (int i = 0; i < ptrNode->curSize; i++)
		{
			ptrNode->key[i] = keyArray[i];
		}
		for (int i = 0; i < newInternal->curSize; i++)
		{
			newInternal->key[i] = keyArray[ptrNode->curSize + i];
		}
		if (ptrNode == rootNode)
		{
			Node* newRoot = new Node;
			newRoot->leaf = false;
			newRoot->key[0] = newInternal->key[0]; //key of new root is smallest key on right node
			newRoot->curSize = 1;
			newRoot->childNode[0] = ptrNode; //set child pointers
			newRoot->childNode[1] = newInternal;
			rootNode = newRoot;
		}
		else
		{
			Node* parent = findParent(rootNode,ptrNode);
			insertInternal(newInternal->key[0], parent, newInternal);
		}
	}
}
Node* BTree::findParent(Node*ptrNode, Node*child)
{
	Node* parent;
	if (ptrNode->leaf || (ptrNode->childNode[0])->leaf) //stop recurssion when hit last 2 levels
	{
		return NULL;
	}
	for (int i = 0; i < ptrNode->curSize + 1; i++)//iterate through all pointers in node
	{
		if (ptrNode->childNode[i] == child) 
		{
			parent = ptrNode;
			return parent;
		}
		else // if not parent, call findParent recursively
		{
			parent = findParent(ptrNode->childNode[i], child);
			if (parent != NULL)
				return parent;
		}
	}
	return parent;
}

void BTree::deleteKey(int deleteKey)
{
	if (rootNode == NULL)
	{
		return;
	}
	else
	{
		Node* ptrNode = rootNode;
		Node* parent;
		int leftSibling, rightSibling;
		int pointer = 0;
		while (ptrNode->leaf == false) //find the leaf node with delete key
		{
			
			for (int i = 0; i < ptrNode->curSize; i++)
			{
				if (deleteKey > ptrNode->key[i])
				{
					pointer = i;
					leftSibling = pointer - 1;
					rightSibling = pointer + 1;
				}
			}
			parent = ptrNode;
			ptrNode = ptrNode->childNode[pointer];
		}
		int keyPointer = 0;
		bool keyExist = false;
		for (int i = 0;i < ptrNode->curSize; i++)//find key in leaf node
		{
			if (deleteKey == ptrNode->key[i])
			{
				keyPointer = i;
				keyExist = true;
				break;
			}
		}
		if (keyExist)
		{
			ptrNode->key[keyPointer] = NULL; //delete key
			ptrNode->curSize = ptrNode->curSize - 1;
			if (ptrNode->curSize > floor((nodeSize + 1) / 2)) //if enough keys in node after deleting
			{
				for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
				{
					ptrNode->key[i] = ptrNode->key[i + 1];
				}
				if (pointer > 0 && keyPointer == 0)
				{
					parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
				}
				return;
			}
			else
			{
				Node* rightSiblingNode = NULL;
				Node* leftSiblingNode = NULL;
				if (leftSibling >= 0) //if left sibling exist
					Node* leftSiblingNode = parent->childNode[leftSibling];
				if (rightSibling < nodeSize)//if right sibling exist
					Node* rightSiblingNode = parent->childNode[rightSibling];
					
				
				if ((leftSiblingNode->curSize-1 > floor((nodeSize + 1) / 2))&&leftSiblingNode!=NULL)//if left sibling can share keys
				{
					int shareKey = leftSiblingNode->key[leftSiblingNode->curSize];
					int insertPtr=0;
					int tempArray[nodeSize];
					for (int i = 0; i < ptrNode->curSize;i++)//squeeze keys in delete node and leave first key empty
					{
						if (ptrNode->key[i] == NULL)
						{
							if (i == 0)
								break;
							else 
							{
								for (int j = i;j >0;j--)
									ptrNode->key[j] = ptrNode->key[j - 1];
								break;
							}
								
						}
					}
						//insert shared key in delete node
						ptrNode->key[0] = shareKey;
						//ptrNode->childNode[0] = leftSiblingNode->childNode[leftSiblingNode->curSize];
						ptrNode->curSize++;
						//delete shared key from sibling
						leftSiblingNode->key[leftSiblingNode->curSize] = NULL;
						//leftSiblingNode->childNode[leftSiblingNode->curSize]=NULL;
						if (pointer > 0 && keyPointer == 0)
						{
							parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
						}
						return;
					}
				else if ((rightSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2))&&rightSiblingNode!=NULL)//if right sibling can share keys
				{
					int shareKey = leftSiblingNode->key[leftSiblingNode->curSize];
					int insertPtr = 0;
					int tempArray[nodeSize];
					for (int i = 0; i < ptrNode->curSize;i++)//squeeze keys in delete node and leave last key empty
					{

						if (ptrNode->key[i] == NULL)
						{
							for (int j = ptrNode->curSize;j > i;j--)
									ptrNode->key[j-1] = ptrNode->key[j];
								break;
						}
					}
					//insert shared key in delete node
					ptrNode->key[ptrNode->curSize] = shareKey;
					//ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[0];
					ptrNode->curSize++;
					//delete shared key from sibling
					//rightSiblingNode->childNode[0] = NULL;
					rightSiblingNode->key[rightSiblingNode->curSize] = NULL;
					if (pointer > 0 && keyPointer == 0)
					{
						parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
					}
					return;
				}
				else //both left and right siblings unable to share keys
				{
					for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
					{
						ptrNode->key[i] = ptrNode->key[i + 1];
					}
					if (leftSiblingNode != NULL)
					{
						for (int i = leftSiblingNode->curSize,int j=0;i < nodeSize,j<ptrNode->curSize;i++,j++)
						{
							//transfer all keys into left sibling
							leftSiblingNode[i] = ptrNode[j];
						}
						//shift last ptr over to left sibling node
						leftSiblingNode->childNode[nodeSize + 1] = ptrNode->childNode[nodeSize + 1];
						leftSiblingNode->curSize += ptrNode->curSize;	
						delete ptrNode;
						deleteInternal(parent->key[leftSibling], parent, leftSiblingNode);
						return;
					}
					else if (rightSiblingNode != NULL)
					{
						for (int i = ptrNode->curSize, int j = 0;i < nodeSize, j < rightSiblingNode->curSize;i++, j++)
						{
							//transfer all keys from right sibling
							ptrNode[i]=rightSiblingNode[j];
						}
						//shift last ptr over from right sibling node
						ptrNode->childNode[nodeSize + 1] = rightSiblingNode->childNode[nodeSize + 1];
						ptrNode->curSize += rightSiblingNode->curSize;
						delete rightSiblingNode;
						deleteInternal(parent->key[rightSibling-1], parent, ptrNode);
					}
				}
				
				
			}
		}
		

	}
}

void BTree::deleteInternal(int deleteKey, Node* ptrNode, Node* child)
{
	if (ptrNode == rootNode)
	{
		if (ptrNode->curSize == 1)
		{
			rootNode = child;
			delete ptrNode;
			return;
		}
	}
	else
	{
		int keyPointer;
		for (int i = 0;i < ptrNode->curSize; i++)//find key in internal node
		{
			if (deleteKey == ptrNode->key[i])
			{
				keyPointer = i;
				break;
			}
		}
		ptrNode->key[keyPointer] = NULL; //delete key
		ptrNode->curSize = ptrNode->curSize - 1;
		if (ptrNode->curSize > floor((nodeSize + 1) / 2)) //if enough keys in node after deleting
		{
			for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
			{
				ptrNode->key[i] = ptrNode->key[i + 1];
				ptrNode->childNode[i] = ptrNode->childNode[i+1];
			}

		}
		int pointer, leftSibling, rightSibling;
		Node* parent = findParent(ptrNode, child);
		for (int i = 0; i < parent->curSize; i++)
		{
			if (deleteKey > ptrNode->key[i])
			{
				pointer = i;
				leftSibling = pointer - 1;
				rightSibling = pointer + 1;
			}
		}
		Node* rightSiblingNode = NULL;
		Node* leftSiblingNode = NULL;
		if (leftSibling >= 0) //if left sibling exist
			Node* leftSiblingNode = parent->childNode[leftSibling];
		if (rightSibling < nodeSize)//if right sibling exist
			Node* rightSiblingNode = parent->childNode[rightSibling];


		if ((leftSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) && leftSiblingNode != NULL)//if left sibling can share keys
		{
			int shareKey = leftSiblingNode->key[leftSiblingNode->curSize-1];
			int insertPtr = 0;
			for (int i = 0; i < ptrNode->curSize;i++)//squeeze keys in delete node and leave first key empty
			{
				if (ptrNode->key[i] == NULL)
				{
					if (i == 0)
						break;
					else
					{
						for (int j = i;j > 0;j--)
							ptrNode->key[j] = ptrNode->key[j - 1];
						break;
					}

				}
			}
			//insert shared key in delete node
			ptrNode->key[0] = shareKey;
			ptrNode->childNode[0] = leftSiblingNode->childNode[leftSiblingNode->curSize];
			ptrNode->curSize++;
			//delete shared key from sibling
			leftSiblingNode->key[leftSiblingNode->curSize-1] = NULL;
			leftSiblingNode->childNode[leftSiblingNode->curSize-1]=NULL;
			if (pointer > 0 && keyPointer == 0)
			{
				parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
			}
			return;
		}
		else if ((rightSiblingNode->curSize - 1 > floor((nodeSize + 1) / 2)) && rightSiblingNode != NULL)//if right sibling can share keys
		{
			int shareKey = leftSiblingNode->key[leftSiblingNode->curSize-1];
			int insertPtr = 0;
			int tempArray[nodeSize];
			for (int i = 0; i < ptrNode->curSize;i++)//squeeze keys in delete node and leave last key empty
			{

				if (ptrNode->key[i] == NULL)
				{
					for (int j = ptrNode->curSize;j > i;j--)
						ptrNode->key[j - 1] = ptrNode->key[j];
					break;
				}
			}
			//insert shared key in delete node
			ptrNode->key[ptrNode->curSize-1] = shareKey;
			ptrNode->childNode[ptrNode->curSize] = rightSiblingNode->childNode[0];
			ptrNode->curSize++;
			//delete shared key from sibling
			rightSiblingNode->childNode[0] = NULL;
			rightSiblingNode->key[rightSiblingNode->curSize] = NULL;
			if (pointer > 0 && keyPointer == 0)
			{
				parent->key[pointer - 1] = ptrNode->key[0]; //update parent key if neccessary
			}
			return;
		}
		else //both left and right siblings unable to share keys
		{
			for (int i = keyPointer; i < ptrNode->curSize; i++) //squeeze keys in node
			{
				ptrNode->key[i] = ptrNode->key[i + 1];
			}
			if (leftSiblingNode != NULL)
			{
				for (int i = leftSiblingNode->curSize, int j = 0;i < nodeSize, j < ptrNode->curSize;i++, j++)
				{
					//transfer all keys into left sibling
					leftSiblingNode[i] = ptrNode[j];
				}
				//shift last ptr over to left sibling node
				leftSiblingNode->childNode[nodeSize + 1] = ptrNode->childNode[nodeSize + 1];
				leftSiblingNode->curSize += ptrNode->curSize;
				delete ptrNode;
				deleteInternal(parent->key[leftSibling], parent, leftSiblingNode);
				return;
			}
			else if (rightSiblingNode != NULL)
			{
				for (int i = ptrNode->curSize, int j = 0;i < nodeSize, j < rightSiblingNode->curSize;i++, j++)
				{
					//transfer all keys from right sibling
					ptrNode[i] = rightSiblingNode[j];
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