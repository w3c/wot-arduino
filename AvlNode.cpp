/* AvlTrees - a variant on balanced binary trees */

#include <Arduino.h>
#include "AvlNode.h"

#define MAX(x, y) (((x) > (y))?(x):(y))

#define AVLNODE(i) (pool+i)

// initialise memory pool for allocating nodes
AvlNode * AvlNode::pool = 0;
unsigned int AvlNode::length = 0;
unsigned int AvlNode::size = 0;

void AvlNode::initialise_pool(AvlNode *buffer, unsigned int size)
{
  pool = buffer;  // memory for size nodes
  length = size;  // number of possible nodes
  size = 0;  // index for allocation next node
}

float AvlNode::used()
{
    // return percentage of allocated nodes
    return 100.0 * size / (1.0 * length);
}

AvlNode * AvlNode::get_node(AvlIndex index)
{
    return AVLNODE(index);
}

AvlKey AvlNode::get_key()
{
    return key;
}

AvlValue AvlNode::get_value()
{
    return value;
}

// for debugging - remove to save space
void AvlNode::print_keys(AvlIndex tree)
{
    if (tree != AVL_NULL)
    {
        print_keys(AVLNODE(tree)->left);
        PRINT(F("  "));
        PRINTLN((int)AVLNODE(tree)->key);
        print_keys(AVLNODE(tree)->right);
    }
}

void AvlNode::apply(AvlIndex tree, AvlApplyFn applyFn, void *data)
{
    if (tree != AVL_NULL)
    {
        apply(AVLNODE(tree)->left, applyFn, data);
        (*applyFn)(AVLNODE(tree)->key, AVLNODE(tree)->value, data);
        apply(AVLNODE(tree)->right, applyFn, data);
    }
}

unsigned int AvlNode::get_size(AvlIndex tree)
{
    if (tree != AVL_NULL)
        return get_size(AVLNODE(tree)->left) + 1 + get_size(AVLNODE(tree)->right);

    return 0;
}

// allocate node from fixed memory pool
AvlIndex AvlNode::new_node(AvlKey key, AvlValue value)
{
    AvlIndex index = AVL_NULL;
    AvlNode *node;
    
    if (pool && size < length)
    {
        index = size++;
        node = AVLNODE(index);
        node->key = key;
        node->value = value;
        node->height = 1;
        node->left = node->right = AVL_NULL;
    }

    return index;
}

AvlValue AvlNode::find_key(AvlIndex tree, AvlKey key)
{
    while (tree != AVL_NULL)
    {
        if (key == AVLNODE(tree)->key)
            return AVLNODE(tree)->value;

        tree = (key < AVLNODE(tree)->key ? AVLNODE(tree)->left : AVLNODE(tree)->right);
    }

    return 0;
}

AvlIndex AvlNode::first(AvlIndex tree)
{
    if (tree != AVL_NULL)
    {
        while (AVLNODE(tree)->left != AVL_NULL)
            tree = AVLNODE(tree)->left;
    }
    return tree;
}

AvlIndex AvlNode::last(AvlIndex tree)
{
    if (tree != AVL_NULL)
    {
        while (AVLNODE(tree)->right != AVL_NULL)
            tree = AVLNODE(tree)->right;
    }
    
    return tree;
}

int AvlNode::tree_height(AvlIndex tree)
{
    if (tree == AVL_NULL)
        return 0;

    int lh = tree_height(AVLNODE(tree)->left);
    int rh = tree_height(AVLNODE(tree)->right);

    return 1 + MAX(lh, rh);
}

int AvlNode::node_height(AvlIndex node)
{
    return (node != AVL_NULL ? AVLNODE(node)->height : 0);
}

// rebalancing operations from wikipedia diagram
// see http://en.wikipedia.org/wiki/AVL_tree
AvlIndex AvlNode::left_left(AvlIndex p5)
{
    //printf("applying left left\n");
    AvlIndex p3 = AVLNODE(p5)->left;
    AVLNODE(p5)->left = AVLNODE(p3)->right;
    AVLNODE(p3)->right = p5;
    AVLNODE(p5)->height = max(node_height(AVLNODE(p5)->left), node_height(AVLNODE(p5)->right)) + 1;
    AVLNODE(p3)->height = max(node_height(AVLNODE(p3)->left), AVLNODE(p5)->height) + 1;
    return p3;
}

AvlIndex AvlNode::right_right(AvlIndex p3)
{
    //printf("applying right right\n");
    AvlIndex p5 = AVLNODE(p3)->right;
    AVLNODE(p3)->right = AVLNODE(p5)->left;
    AVLNODE(p5)->left = p3;
    AVLNODE(p3)->height = max(node_height(AVLNODE(p3)->left), node_height(AVLNODE(p3)->right)) + 1;
    AVLNODE(p5)->height = max(AVLNODE(p3)->height, node_height(AVLNODE(p5)->right)) + 1;
    return p5;
}

AvlIndex AvlNode::left_right(AvlIndex p5)
{
    //printf("applying left right\n");
    AVLNODE(p5)->left = right_right(AVLNODE(p5)->left);
    return left_left(p5);
}

AvlIndex AvlNode::right_left(AvlIndex p3)
{
    //printf("applying right left\n");
    AVLNODE(p3)->right = left_left(AVLNODE(p3)->right);
    return right_right(p3);
}

int AvlNode::get_balance(AvlIndex tree)
{
    int balance = 0;

    if (tree != AVL_NULL)
    {
        if (AVLNODE(tree)->left != AVL_NULL)
            balance -= AVLNODE(AVLNODE(tree)->left)->height;

        if (AVLNODE(tree)->right != AVL_NULL)
            balance += AVLNODE(AVLNODE(tree)->right)->height;
    }

    return balance;
}

AvlIndex AvlNode::balance(AvlIndex tree)
{
    int balance, lh, rh;

    balance = get_balance(tree);

    if (balance < -1)
    {
        balance = get_balance(AVLNODE(tree)->left);

        if (balance > 0)
            tree = left_right(tree);
        else
            tree = left_left(tree);
    }
    else if (balance > 1)
    {
        balance = get_balance(AVLNODE(tree)->right);

        if (balance < 0)
            tree = right_left(tree);
        else
            tree = right_right(tree);
    }

    lh = node_height(AVLNODE(tree)->left);
    rh = node_height(AVLNODE(tree)->right);
    AVLNODE(tree)->height = (lh > rh ? lh : rh) + 1;
    return tree;
}

AvlIndex AvlNode::insert_key(AvlIndex tree, AvlKey key, AvlValue value)
{
    if (tree == AVL_NULL)
    {
        tree = new_node(key, value);
        return tree;
    }

    if (key < AVLNODE(tree)->key)
    {
        AVLNODE(tree)->left = insert_key(AVLNODE(tree)->left, key, value);
        tree = balance(tree);
    }
    else if (key > AVLNODE(tree)->key)
    {
        AVLNODE(tree)->right = insert_key(AVLNODE(tree)->right, key, value);
        tree = balance(tree);
    }
    /* otherwise tree already has key */

    return tree;
}

AvlIndex AvlNode::mk_node(AvlIndex left, AvlKey key, AvlValue value, AvlIndex right)
{
    AvlIndex tree = new_node(key, value);
    AVLNODE(tree)->right = right;
    AVLNODE(tree)->height = max(node_height(AVLNODE(tree)->left),
                                 node_height(AVLNODE(tree)->right)) + 1;
    return tree;
}
