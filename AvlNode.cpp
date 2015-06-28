/* AvlTrees - a variant on balanced binary trees */

#include "AvlNode.h"

#ifndef NULL
#define NULL 0
#endif

#define MAX(x, y) (((x) > (y))?(x):(y))

// initialise memory pool for allocating nodes
AvlNode *AvlNode::pool = NULL;
unsigned int AvlNode::length = 0;
unsigned int AvlNode::size = 0;

void AvlNode::initialise_pool(AvlNode *buffer, unsigned int size)
{
  pool = buffer;  // memory for size nodes
  length = size;  // number of possible nodes
  size = 0;  // index for allocation next node
}

// allocate node from fixed memory pool
AvlNode * AvlNode::newNode(AvlKey key, AvlValue value)
{
    AvlNode * node = NULL;
    
    if (pool && size < length)
    {
        node = pool + size++;
        node->key = key;
        node->value = value;
        node->height = 1;
        node->left = node->right = NULL;
    }
    
    return node;
}

AvlNode *AvlNode::avlFindKey(AvlNode *tree, AvlKey key)
{
    while (tree)
    {
        if (key == tree->key)
            return tree;

        tree = (key < tree->key ? tree->left : tree->right);
    }

    return NULL;
}

AvlNode *AvlNode::avlFirst(AvlNode *tree)
{
    if (tree)
    {
        while (tree->left)
            tree = tree->left;
    }
    return tree;
}

AvlNode *AvlNode::avlLast(AvlNode *tree)
{
    if (tree)
    {
        while (tree->right)
            tree = tree->right;
    }
    return tree;
}

int AvlNode::avlTreeHeight(AvlNode *tree)
{
    if (!tree) return 0;

    int lh = avlTreeHeight(tree->left);
    int rh = avlTreeHeight(tree->right);

    return 1 + MAX(lh, rh);
}

int AvlNode::avlNodeHeight(AvlNode *node)
{
    return (node ? node->height : 0);
}

int max(int x, int y)
{
    return (x > y ? x : y);
}

// rebalancing operations from wikipedia diagram
// see http://en.wikipedia.org/wiki/AVL_tree
AvlNode *AvlNode::avlLeftLeft(AvlNode *p5)
{
    //printf("applying left left\n");
    AvlNode *p3 = p5->left;
    p5->left = p3->right;
    p3->right = p5;
    p5->height = max(avlNodeHeight(p5->left), avlNodeHeight(p5->right)) + 1;
    p3->height = max(avlNodeHeight(p3->left), p5->height) + 1;
    return p3;
}

AvlNode *AvlNode::avlRightRight(AvlNode *p3)
{
    //printf("applying right right\n");
    AvlNode *p5 = p3->right;
    p3->right = p5->left;
    p5->left = p3;
    p3->height = max(avlNodeHeight(p3->left), avlNodeHeight(p3->right)) + 1;
    p5->height = max(p3->height, avlNodeHeight(p5->right)) + 1;
    return p5;
}

AvlNode *AvlNode::avlLeftRight(AvlNode *p5)
{
    //printf("applying left right\n");
    p5->left = avlRightRight(p5->left);
    return avlLeftLeft(p5);
}

AvlNode *AvlNode::avlRightLeft(AvlNode *p3)
{
    //printf("applying right left\n");
    p3->right = avlLeftLeft(p3->right);
    return avlRightRight(p3);
}

int AvlNode::avlGetBalance(AvlNode *tree)
{
    int balance = 0;

    if (tree)
    {
        if (tree->left)
            balance -= tree->left->height;

        if (tree->right)
            balance += tree->right->height;
    }

    return balance;
}

AvlNode *AvlNode::avlBalance(AvlNode *tree)
{
    int balance, lh, rh;

    balance = avlGetBalance(tree);

    if (balance < -1)
    {
        balance = avlGetBalance(tree->left);

        if (balance > 0)
            tree = avlLeftRight(tree);
        else
            tree = avlLeftLeft(tree);
    }
    else if (balance > 1)
    {
        balance = avlGetBalance(tree->right);

        if (balance < 0)
            tree = avlRightLeft(tree);
        else
            tree = avlRightRight(tree);
    }

    lh = avlNodeHeight(tree->left);
    rh = avlNodeHeight(tree->right);
    tree->height = (lh > rh ? lh : rh) + 1;
    return tree;
}

AvlNode *AvlNode::avlInsertKey(AvlNode *tree, AvlKey key, AvlValue value)
{
    if (!tree)
    {
        //tree = new AvlNode(key, value);
        
        tree = newNode(key, value);
        return tree;
    }

    if (key < tree->key)
    {
        tree->left = avlInsertKey(tree->left, key, value);
        tree = avlBalance(tree);
    }
    else if (key > tree->key)
    {
        tree->right = avlInsertKey(tree->right, key, value);
        tree = avlBalance(tree);
    }
    /* otherwise tree already has key */

    return tree;
}

AvlNode *AvlNode::mkAvlNode(AvlNode *left, AvlKey key, AvlValue value, AvlNode *right)
{
    //AvlNode *node = new AvlNode(key, value);
    
    AvlNode *node = newNode(key, value);
    node->right = right;
    node->height = max(avlNodeHeight(node->left), avlNodeHeight(node->right)) + 1;
    return node;
}
