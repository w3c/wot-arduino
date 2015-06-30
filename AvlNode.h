/* AvlTrees - a variant on balanced binary trees */

#include "core.h"

typedef String AvlKey;  // e.g. memory address of object
typedef void *AvlValue;

class AvlNode
{
    public:
        
        static void initialise_pool(AvlNode *pool, unsigned int size);
        static AvlNode * avlFindKey(AvlNode *tree, AvlKey key);
        static AvlNode * avlInsertKey(AvlNode *tree, AvlKey key, AvlValue value);
        static AvlNode * avlFirst(AvlNode *tree);
        static AvlNode * avlLast(AvlNode *tree);
        
    private:
    
        AvlKey key;
        AvlValue value;
        int height;
        AvlNode *left;
        AvlNode *right;
        
        static unsigned int length;
        static unsigned int size;
        static AvlNode *pool;
        
        static AvlNode * newNode(AvlKey key, AvlValue value);
        
        static int avlTreeHeight(AvlNode *tree);
        static int avlNodeHeight(AvlNode *node);
        static AvlNode * avlLeftLeft(AvlNode *p5);
        static AvlNode * avlRightRight(AvlNode *p3);
        static AvlNode * avlLeftRight(AvlNode *p5);
        static AvlNode * avlRightLeft(AvlNode *p3);
        static int avlGetBalance(AvlNode *tree);
        static AvlNode * avlBalance(AvlNode *tree);
        static AvlNode * mkAvlNode(AvlNode *left, AvlKey key, AvlValue value, AvlNode *right);
};
