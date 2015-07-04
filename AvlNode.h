// AvlTrees - a variant on balanced binary trees

#ifndef _WOTF_AVLNODE
#define _WOTF_AVLNODE

#ifndef null
#define null 0
#endif

#define AVL_NULL -1
#define AVL_MAX_INDEX 127

typedef int8_t AvlIndex;  // index into memory pool of AvlNodes
typedef int8_t AvlKey;  // a symbol or array index
typedef void *AvlValue; // e.g. pointer to a JSON object
typedef void (*AvlApplyFn)(AvlKey key, AvlValue value, void *data);

class AvlNode
{
    public:
        
        AvlKey get_key();
        AvlValue get_value();
        
        static void initialise_pool(AvlNode *pool, unsigned int size);
        static float used();
        static AvlNode * get_node(AvlIndex index);
        static AvlValue find_key(AvlIndex tree, AvlKey key);
        static AvlIndex insert_key(AvlIndex tree, AvlKey key, AvlValue value);
        static AvlIndex first(AvlIndex tree);
        static AvlIndex last(AvlIndex tree);
        static unsigned int get_size(AvlIndex tree);
        static void apply(AvlIndex tree, AvlApplyFn applyFn, void *data);
        static void print_keys(AvlIndex tree);
        
    private:
    
        AvlValue value;
        AvlKey key;
        uint8_t height;
        int8_t left;
        int8_t right;
        
        static unsigned int length;
        static unsigned int size;
        static AvlNode *pool;

        static AvlIndex new_node(AvlKey key, AvlValue value);
        
        
        static int tree_height(AvlIndex tree);
        static int node_height(AvlIndex node);
        static AvlIndex left_left(AvlIndex p5);
        static AvlIndex right_right(AvlIndex p3);
        static AvlIndex left_right(AvlIndex p5);
        static AvlIndex right_left(AvlIndex p3);
        static int get_balance(AvlIndex tree);
        static AvlIndex balance(AvlIndex tree);
        static AvlIndex mk_node(AvlIndex left, AvlKey key, AvlValue value, AvlIndex right);
};

#endif