/* AvlTrees - a variant on balanced binary trees */

#include "AvlNode.h"

#define MAX(x, y) (((x) > (y))?(x):(y))

// initialise memory pool for allocating nodes
AvlNode *AvlNode::pool = 0;
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

AvlKey AvlNode::get_key()
{
    return key;
}

AvlValue AvlNode::get_value()
{
    return value;
}

// for debugging - remove to save space
void AvlNode::print_keys(AvlNode *tree)
{
    if (tree)
    {
        print_keys(tree->left);
        cout << "  " << tree->key << "\n";
        print_keys(tree->right);
    }
}

void AvlNode::apply(AvlNode *tree, AvlApplyFn applyFn, void *data)
{
    if (tree)
    {
        apply(tree->left, applyFn, data);
        (*applyFn)(tree->key, tree->value, data);
        apply(tree->right, applyFn, data);
    }
}

unsigned int AvlNode::get_size(AvlNode *tree)
{
    if (tree)
        return get_size(tree->left) + 1 + get_size(tree->right);

    return 0;
}

// allocate node from fixed memory pool
AvlNode * AvlNode::new_node(AvlKey key, AvlValue value)
{
    AvlNode * node = 0;
    
    if (pool && size < length)
    {
        node = pool + size++;
        node->key = key;
        node->value = value;
        node->height = 1;
        node->left = node->right = 0;
    }
    
    return node;
}

AvlNode *AvlNode::find_key(AvlNode *tree, AvlKey key)
{
    while (tree)
    {
        if (key == tree->key)
            return tree;

        tree = (key < tree->key ? tree->left : tree->right);
    }

    return 0;
}

AvlNode *AvlNode::first(AvlNode *tree)
{
    if (tree)
    {
        while (tree->left)
            tree = tree->left;
    }
    return tree;
}

AvlNode *AvlNode::last(AvlNode *tree)
{
    if (tree)
    {
        while (tree->right)
            tree = tree->right;
    }
    return tree;
}

int AvlNode::tree_height(AvlNode *tree)
{
    if (!tree) return 0;

    int lh = tree_height(tree->left);
    int rh = tree_height(tree->right);

    return 1 + MAX(lh, rh);
}

int AvlNode::node_height(AvlNode *node)
{
    return (node ? node->height : 0);
}

int max(int x, int y)
{
    return (x > y ? x : y);
}

// rebalancing operations from wikipedia diagram
// see http://en.wikipedia.org/wiki/AVL_tree
AvlNode *AvlNode::left_left(AvlNode *p5)
{
    //printf("applying left left\n");
    AvlNode *p3 = p5->left;
    p5->left = p3->right;
    p3->right = p5;
    p5->height = max(node_height(p5->left), node_height(p5->right)) + 1;
    p3->height = max(node_height(p3->left), p5->height) + 1;
    return p3;
}

AvlNode *AvlNode::right_right(AvlNode *p3)
{
    //printf("applying right right\n");
    AvlNode *p5 = p3->right;
    p3->right = p5->left;
    p5->left = p3;
    p3->height = max(node_height(p3->left), node_height(p3->right)) + 1;
    p5->height = max(p3->height, node_height(p5->right)) + 1;
    return p5;
}

AvlNode *AvlNode::left_right(AvlNode *p5)
{
    //printf("applying left right\n");
    p5->left = right_right(p5->left);
    return left_left(p5);
}

AvlNode *AvlNode::right_left(AvlNode *p3)
{
    //printf("applying right left\n");
    p3->right = left_left(p3->right);
    return right_right(p3);
}

int AvlNode::get_balance(AvlNode *tree)
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

AvlNode *AvlNode::balance(AvlNode *tree)
{
    int balance, lh, rh;

    balance = get_balance(tree);

    if (balance < -1)
    {
        balance = get_balance(tree->left);

        if (balance > 0)
            tree = left_right(tree);
        else
            tree = left_left(tree);
    }
    else if (balance > 1)
    {
        balance = get_balance(tree->right);

        if (balance < 0)
            tree = right_left(tree);
        else
            tree = right_right(tree);
    }

    lh = node_height(tree->left);
    rh = node_height(tree->right);
    tree->height = (lh > rh ? lh : rh) + 1;
    return tree;
}

AvlNode *AvlNode::insert_key(AvlNode *tree, AvlKey key, AvlValue value)
{
    if (!tree)
    {
        //tree = new AvlNode(key, value);
        
        tree = new_node(key, value);
        return tree;
    }

    if (key < tree->key)
    {
        tree->left = insert_key(tree->left, key, value);
        tree = balance(tree);
    }
    else if (key > tree->key)
    {
        tree->right = insert_key(tree->right, key, value);
        tree = balance(tree);
    }
    /* otherwise tree already has key */

    return tree;
}

AvlNode *AvlNode::mk_node(AvlNode *left, AvlKey key, AvlValue value, AvlNode *right)
{
    AvlNode *node = new_node(key, value);
    node->right = right;
    node->height = max(node_height(node->left), node_height(node->right)) + 1;
    return node;
}
