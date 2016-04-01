#ifndef _WOT_NODE_POOL
#define _WOT_NODE_POOL

#define WOT_NODE_POOL_SIZE 80

typedef uint8_t NPIndex;

typedef struct {
    char byte[4];
    void *pointer;
} wot_node_pool_t;

class WotNodePool
{
    public:
        WotNodePool();
        wot_node_pool_t wot_pool[WOT_NODE_POOL_SIZE];
        unsigned int used;
        unsigned int last_allocated;
        boolean gc_phase;

        void initialise_node_pool(wot_node_pool_t *buffer, unsigned int size);
        unsigned int size();
        float percent_used();
        void *allocate_node();
        void *get_node(unsigned int index);
        wot_node_pool_t *get_pool();
        void free(void *node);
};
#endif