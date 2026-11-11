#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
bool checkLeafDepth(Node* node, int depth, int& leafDepth)
{
	if(node == nullptr) {
        return true;
    }

    if(node->left == nullptr && node->right == nullptr) {
        if(leafDepth == -1) {
            leafDepth = depth;
        }

        return depth == leafDepth;
    }

    bool leftEqual = checkLeafDepth(
        node->left, depth + 1, leafDepth);

    if(leftEqual == false) {
        return false;
    }

    return checkLeafDepth(node->right, depth + 1, leafDepth);
}

bool equalPaths(Node * root)
{
    // Add your code below
		int leafDepth = -1;
		return checkLeafDepth(root, 0, leafDepth);
}

