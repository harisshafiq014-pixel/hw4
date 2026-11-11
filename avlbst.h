#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
		using BinarySearchTree<Key, Value>::nodeSwap;

		AVLNode<Key, Value>* rotateLeft(AVLNode<Key, Value>* node);
		AVLNode<Key, Value>* rotateRight(AVLNode<Key, Value>* node);
		void insertFix(AVLNode<Key, Value>* node);
		void removeFix(AVLNode<Key, Value>* node, int diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    if(this->root_ == nullptr) {
        this->root_ = new AVLNode<Key, Value>(
            new_item.first, new_item.second, nullptr);
        return;
    }

    AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->root_);

    while(current != nullptr) {
        if(new_item.first < current->getKey()) {
            if(current->getLeft() == nullptr) {
                AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
                    new_item.first, new_item.second, current);
                current->setLeft(node);
                insertFix(node);
                return;
            }
            current = current->getLeft();
        }
        else if(current->getKey() < new_item.first) {
            if(current->getRight() == nullptr) {
                AVLNode<Key, Value>* node = new AVLNode<Key, Value>(
                    new_item.first, new_item.second, current);
                current->setRight(node);
                insertFix(node);
                return;
            }
            current = current->getRight();
        }
        else {
            current->setValue(new_item.second);
            return;
        }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
      AVLNode<Key, Value>* current =
        static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    if(current == nullptr) {
        return;
    }

    if(current->getLeft() != nullptr && current->getRight() != nullptr) {
        AVLNode<Key, Value>* pred =
            static_cast<AVLNode<Key, Value>*>(
                this->predecessor(current));
        nodeSwap(current, pred);
    }

    AVLNode<Key, Value>* parent = current->getParent();
    AVLNode<Key, Value>* child = current->getLeft();

    if(child == nullptr) {
        child = current->getRight();
    }

    int diff = 0;
    if(parent != nullptr) {
        diff = (parent->getLeft() == current) ? 1 : -1;
    }

    if(child != nullptr) {
        child->setParent(parent);
    }

    if(parent == nullptr) {
        this->root_ = child;
    }
    else if(parent->getLeft() == current) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    delete current;
    removeFix(parent, diff);
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* child = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(child->getLeft());
    if(child->getLeft() != nullptr) {
        child->getLeft()->setParent(node);
    }

    child->setParent(parent);
    if(parent == nullptr) {
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    child->setLeft(node);
    node->setParent(child);

    int nodeBalance = node->getBalance() - 1
        - std::max(0, static_cast<int>(child->getBalance()));
    int childBalance = child->getBalance() - 1
        + std::min(0, nodeBalance);

    node->setBalance(nodeBalance);
    child->setBalance(childBalance);

    return child;
}

template<class Key, class Value>
AVLNode<Key, Value>*
AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* child = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(child->getRight());
    if(child->getRight() != nullptr) {
        child->getRight()->setParent(node);
    }

    child->setParent(parent);
    if(parent == nullptr) {
        this->root_ = child;
    }
    else if(parent->getLeft() == node) {
        parent->setLeft(child);
    }
    else {
        parent->setRight(child);
    }

    child->setRight(node);
    node->setParent(child);

    int nodeBalance = node->getBalance() + 1
        - std::min(0, static_cast<int>(child->getBalance()));
    int childBalance = child->getBalance() + 1
        + std::max(0, nodeBalance);

    node->setBalance(nodeBalance);
    child->setBalance(childBalance);

    return child;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* parent = node->getParent();

    while(parent != nullptr) {
        parent->updateBalance(node == parent->getLeft() ? -1 : 1);

        if(parent->getBalance() == 0) {
            return;
        }

        if(parent->getBalance() == -2) {
            if(parent->getLeft()->getBalance() > 0) {
                rotateLeft(parent->getLeft());
            }
            rotateRight(parent);
            return;
        }

        if(parent->getBalance() == 2) {
            if(parent->getRight()->getBalance() < 0) {
                rotateRight(parent->getRight());
            }
            rotateLeft(parent);
            return;
        }

        node = parent;
        parent = parent->getParent();
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(
    AVLNode<Key, Value>* node, int diff)
{
    while(node != nullptr) {
        node->updateBalance(diff);

        if(node->getBalance() == -1 || node->getBalance() == 1) {
            return;
        }

        AVLNode<Key, Value>* newRoot = node;

        if(node->getBalance() == -2) {
            int childBalance = node->getLeft()->getBalance();

            if(childBalance > 0) {
                rotateLeft(node->getLeft());
            }

            newRoot = rotateRight(node);

            if(childBalance == 0) {
                return;
            }
        }
        else if(node->getBalance() == 2) {
            int childBalance = node->getRight()->getBalance();

            if(childBalance < 0) {
                rotateRight(node->getRight());
            }

            newRoot = rotateLeft(node);

            if(childBalance == 0) {
                return;
            }
        }

        AVLNode<Key, Value>* parent = newRoot->getParent();
        if(parent == nullptr) {
            return;
        }

        diff = (newRoot == parent->getLeft()) ? 1 : -1;
        node = parent;
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}



#endif
