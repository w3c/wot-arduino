/* AvlTrees - a variant on balanced binary trees */

#include <Arduino.h>
#include "NodePool.h"
#include "AvlNode.h"

#define MAX(x, y) (((x) > (y))?(x):(y))

#define AVLNODE(i) ((AvlNode *)(node_pool + i - 1))
#define AVLINDEX(node) ((AvlNode *)((wot_node_pool_t *)node - node_pool + 1));

// initialise memory pool for allocating nodes
static wot_node_pool_t *node_pool;
static WotNodePool *node_pool_manager;

void AvlNode::initialise_pool(WotNodePool *pool)
{
    node_pool_manager = pool;
    node_pool = pool->get_pool();
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

void AvlNode::print_keys(AvlIndex tree)
{
    if (tree)
    {
        print_keys(AVLNODE(tree)->left);
        Serial.print(F("  "));
        Serial.println((int)AVLNODE(tree)->key);
        print_keys(AVLNODE(tree)->right);
    }
}

void AvlNode::print(AvlIndex tree)
{
    if (tree)
    {
        print(AVLNODE(tree)->left);
        Serial.print(F("  "));
        Serial.print((unsigned int)AVLNODE(tree)->key);
        Serial.print(F(" : "));
        Serial.println((unsigned long)AVLNODE(tree)->value);
        print(AVLNODE(tree)->right);
    }
}

void AvlNode::apply(AvlIndex tree, AvlApplyFn applyFn, void *data)
{
    if (tree)
    {
        apply(AVLNODE(tree)->left, applyFn, data);
        (*applyFn)(AVLNODE(tree)->key, AVLNODE(tree)->value, data);
        apply(AVLNODE(tree)->right, applyFn, data);
    }
}

unsigned int AvlNode::get_size(AvlIndex tree)
{
    if (tree)
        return get_size(AVLNODE(tree)->left) + 1 + get_size(AVLNODE(tree)->right);

    return 0;
}

void AvlNode::free(AvlIndex tree)
{
    if (tree)
    {
        free(AVLNODE(tree)->left);
        free(AVLNODE(tree)->right);
        node_pool_manager->free(AVLNODE(tree));
        Serial.println(F("freeing AVL node"));
    }
}

// allocate node from fixed memory pool
AvlIndex AvlNode::new_node(AvlKey key, AvlValue value)
{
    AvlNode *node = (AvlNode *)(node_pool_manager->allocate_node());
    AvlIndex index = 0;

    if (node)
    {
        Serial.println(F("allocating AVL node"));
        index = node - ((AvlNode *)node_pool) + 1;
        node->key = key;
        node->value = value;
        node->height = 1;
        node->left = node->right = 0;
    }
    else
        Serial.println(F("Out of memory for JSON Node pool"));

    return index;
}

AvlValue AvlNode::find_key(AvlIndex tree, AvlKey key)
{
    while (tree)
    {
        if (key == AVLNODE(tree)->key)
            return AVLNODE(tree)->value;

        tree = (key < AVLNODE(tree)->key ? AVLNODE(tree)->left : AVLNODE(tree)->right);
    }

    return 0;
}

AvlIndex AvlNode::first(AvlIndex tree)
{
    if (tree)
    {
        while (AVLNODE(tree)->left)
            tree = AVLNODE(tree)->left;
    }
    return tree;
}

AvlIndex AvlNode::last(AvlIndex tree)
{
    if (tree)
    {
        while (AVLNODE(tree)->right)
            tree = AVLNODE(tree)->right;
    }
    
    return tree;
}

AvlKey AvlNode::last_key(AvlIndex tree)
{
    AvlIndex i = last(tree);
    
    if (i)
        return AVLNODE(i)->key;
    
    return 0;
}

int AvlNode::tree_height(AvlIndex tree)
{
    if (!tree)
        return 0;

    int lh = tree_height(AVLNODE(tree)->left);
    int rh = tree_height(AVLNODE(tree)->right);

    return 1 + MAX(lh, rh);
}

int AvlNode::node_height(AvlIndex node)
{
    return (node ? AVLNODE(node)->height : 0);
}

// rebalancing operations from wikipedia diagram
// see http://en.wikipedia.org/wiki/AVL_tree
AvlIndex AvlNode::left_left(AvlIndex p5)
{
    AvlIndex p3 = AVLNODE(p5)->left;
    AVLNODE(p5)->left = AVLNODE(p3)->right;
    AVLNODE(p3)->right = p5;
    AVLNODE(p5)->height = max(node_height(AVLNODE(p5)->left), node_height(AVLNODE(p5)->right)) + 1;
    AVLNODE(p3)->height = max(node_height(AVLNODE(p3)->left), AVLNODE(p5)->height) + 1;
    return p3;
}

AvlIndex AvlNode::right_right(AvlIndex p3)
{
    AvlIndex p5 = AVLNODE(p3)->right;
    AVLNODE(p3)->right = AVLNODE(p5)->left;
    AVLNODE(p5)->left = p3;
    AVLNODE(p3)->height = max(node_height(AVLNODE(p3)->left), node_height(AVLNODE(p3)->right)) + 1;
    AVLNODE(p5)->height = max(AVLNODE(p3)->height, node_height(AVLNODE(p5)->right)) + 1;
    return p5;
}

AvlIndex AvlNode::left_right(AvlIndex p5)
{
    AVLNODE(p5)->left = right_right(AVLNODE(p5)->left);
    return left_left(p5);
}

AvlIndex AvlNode::right_left(AvlIndex p3)
{
    AVLNODE(p3)->right = left_left(AVLNODE(p3)->right);
    return right_right(p3);
}

int AvlNode::get_balance(AvlIndex tree)
{
    int balance = 0;

    if (tree)
    {
        if (AVLNODE(tree)->left)
            balance -= AVLNODE(AVLNODE(tree)->left)->height;

        if (AVLNODE(tree)->right)
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
    if (!tree)
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
    else // tree already has key
        AVLNODE(tree)->value = value;

    return tree;
}
