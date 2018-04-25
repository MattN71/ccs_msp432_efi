#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Linked list using statically allocated memory */
#define SIZE_OF_ALLOCATION 32

typedef enum {start, end, active, inactive} nodeState;


static struct timeNode allocation[SIZE_OF_ALLOCATION];
static uint_fast8_t status[SIZE_OF_ALLOCATION];

//Point to beginning of linked list
struct timeNode *ll_start;

struct timeNode {
    uint32_t val; //time to execute
    void (*action)(void); //Function to execute
    struct timeNode *next; //Next node in linked list
    struct timeNode *prev; //Previous node in linked list
    bool active; //Whether or not this node has been reached or not
};


//Initialize two arrays
void init_linked_list(void) {
    size_t i;
    for (i = 0; i < SIZE_OF_ALLOCATION; i++) {
        status[i] = 0;
        allocation[i].val = UINT32_MAX;
        allocation[i].action = NULL;
        allocation[i].next = NULL;
        allocation[i].prev = NULL;
        allocation[i].active = false;
    }
    ll_start = &allocation[0];
}

struct timeNode* addNode(void) {
    size_t i;
    for (i = 0; i < SIZE_OF_ALLOCATION; i++) {
        if (status[i] == 0) { //If empty space is found
            status[i] = 1; //Mark as used
            return &allocation[i]; //Return pointer
        }
    }
    return NULL;
}

void removeNode(struct timeNode *toRemove) {
    size_t i;
    for (i = 0; i < SIZE_OF_ALLOCATION; i++) {
        if (toRemove == &allocation[i]) { //Once we find the struct to remove
            status[i] = 0; //Mark as available
            break;
        }
    }
    toRemove->active = false; //Update active flag
}

//Inserts node into list in correct positon
//Sets active flag
void insertNode(struct timeNode *toAdd) {
    struct timeNode *temp = ll_start; //Start at beginning of list

    while (temp->val < toAdd->val && temp->active) { //Find next spot in queue. Active flag fixes problem at inception
        temp = (temp->next); //Go to next in list
    }
    /*
     *   _____       ______       _____
     *  |  A  | --> | temp | --> |  B  |
     *  |_____| <-- |______| <-- |_____|
     *           |
     *           | Insert here
     *         _____
     *        |toAdd|
     *        |(new)|
     *        |_____|
     */

    struct timeNode * const a = temp->prev;

    //Insert before temp and adjust next / previous pointers
    toAdd->next = temp; //ok
    toAdd->prev = a; //ok
    a->next = toAdd; //ok
    temp->prev = toAdd; //ok

    toAdd->active = true;
}


