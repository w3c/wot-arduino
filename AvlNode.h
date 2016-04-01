// AvlTrees - a variant on balanced binary trees

#ifndef _WOTF_AVLNODE
#define _WOTF_AVLNODE

#ifndef null
#define null 0
#endif

#define AVL_MAX_INDEX 255

typedef uint8_t AvlIndex; // index into memory pool of AvlNodes
typedef uint8_t AvlKey;  // a symbol or array index
typedef void *AvlValue; // e.g. pointer to a JSON object
typedef void (*AvlApplyFn)(AvlKey key, AvlValue value, void *data);

// key must be a positive integer as 0 is used to denote null
// if at some point we want to  have arrays with both named
// *and* numeric indices, then it will be appropriate to make
// the key into a signed integer with negative values as
// symbols denoting named indices, and positive values as
// numeric indices

class AvlNode
{
    public:
        
        AvlKey get_key();
        AvlValue get_value();
        
        static void initialise_pool(WotNodePool *wot_node_pool);
        static AvlNode * get_node(AvlIndex index);
        static AvlValue find_key(AvlIndex tree, AvlKey key);
        static AvlIndex insert_key(AvlIndex tree, AvlKey key, AvlValue value);
        static AvlIndex first(AvlIndex tree);
        static AvlIndex last(AvlIndex tree);
        static AvlKey last_key(AvlIndex tree);
        static unsigned int get_size(AvlIndex tree);
        static void apply(AvlIndex tree, AvlApplyFn applyFn, void *data);
        static void free(AvlIndex tree);
        static void print_keys(AvlIndex tree);
        static void print(AvlIndex tree);
        
    private:
    
        uint8_t height;
        int8_t left;
        int8_t right;
        AvlKey key;
        AvlValue value;
        
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
};

#endif