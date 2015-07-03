// AvlTrees - a variant on balanced binary trees

#ifndef _WOTF_AVLNODE
#define _WOTF_AVLNODE

#include "core.h"

typedef unsigned int AvlKey;  // e.g. a symbol
typedef void *AvlValue; // e.g. pointer to a JSON object
typedef void (*AvlApplyFn)(AvlKey key, AvlValue value, void *data);

class AvlNode
{
    public:
        
        static void initialise_pool(AvlNode *pool, unsigned int size);
        static float used();
        static AvlNode * find_key(AvlNode *tree, AvlKey key);
        static AvlNode * insert_key(AvlNode *tree, AvlKey key, AvlValue value);
        static AvlNode * first(AvlNode *tree);
        static AvlNode * last(AvlNode *tree);
        static unsigned int get_size(AvlNode *tree);
        static void apply(AvlNode *tree, AvlApplyFn applyFn, void *data);
        static void print_keys(AvlNode *tree);
        
    private:
    
        AvlKey key;
        AvlValue value;
        int height;
        AvlNode *left;
        AvlNode *right;
        
        static unsigned int length;
        static unsigned int size;
        static AvlNode *pool;
        
        static AvlNode * new_node(AvlKey key, AvlValue value);
        
        static int tree_height(AvlNode *tree);
        static int node_height(AvlNode *node);
        static AvlNode * left_left(AvlNode *p5);
        static AvlNode * right_right(AvlNode *p3);
        static AvlNode * left_right(AvlNode *p5);
        static AvlNode * right_left(AvlNode *p3);
        static int get_balance(AvlNode *tree);
        static AvlNode * balance(AvlNode *tree);
        static AvlNode * mk_node(AvlNode *left, AvlKey key, AvlValue value, AvlNode *right);
};

#endif