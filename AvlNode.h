/* AvlTrees - a variant on balanced binary trees */

typedef unsigned int AvlKey;  // e.g. memory address of object
typedef void *AvlValue;

class AvlNode
{
    public:
        AvlNode(AvlKey key, AvlValue value);        
        AvlNode * avlFindKey(AvlNode *tree, AvlKey key);
        AvlNode * avlInsertKey(AvlNode *tree, AvlKey key, AvlValue value);
        AvlNode * avlFirst(AvlNode *tree);
        AvlNode * avlLast(AvlNode *tree);
        
    private:
    
        AvlKey key;
        AvlValue value;
        int height;
        AvlNode *left;
        AvlNode *right;
        
        int avlTreeHeight(AvlNode *tree);
        int avlNodeHeight(AvlNode *node);
        AvlNode * avlLeftLeft(AvlNode *p5);
        AvlNode * avlRightRight(AvlNode *p3);
        AvlNode * avlLeftRight(AvlNode *p5);
        AvlNode * avlRightLeft(AvlNode *p3);
        int avlGetBalance(AvlNode *tree);
        AvlNode * avlBalance(AvlNode *tree);
        AvlNode * mkAvlNode(AvlNode *left, AvlKey key, AvlValue value, AvlNode *right);
};
