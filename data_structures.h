#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stdint.h>
#include <stdlib.h>

typedef enum { false, true } bool;

// Ordered Singly-Linked-List of RTEs nodes, all with the same routing prefix.
typedef struct RTE {
    struct RTE *next;
    char *gw;
    char *intf;
    uint32_t qual;
} RTE;

// IP Lookup Tree Node (symbolising a 0 or 1 in routing prefix) containing 0 to
// 'size' RTEs.
typedef struct node_t {
    struct node_t *zero;
    struct node_t *one;
    // Indicating whether the current bit marks the end of a routing prefix.
    bool complete_prefix;
    // RTE of best interface to forward on to reach the desired network
    // (i.e. the routing prefix).
    RTE *best_RTE;
    // Number of different interfaces/gateways to reach the desired network
    // (i.e. the routing prefix).
    size_t size;
} node_t;

typedef struct {
    // Root node to start of IPv4 Lookup Tree.
    node_t *root_IPv4;
    // Total number of IPv4 RTEs in IPv4 Lookup Tree.
    size_t size_IPv4;

    // Root node to start of IPv6 Lookup Tree.
    node_t *root_IPv6;
    // Total number of IPv6 RTEs in IPv6 Lookup Tree.
    size_t size_IPv6;
} RIB;

/*
 * Relationship of Data-Structures:
 * The Routing-Information-Base (RIB) is stored as two trees. A tree for IPv4
 * and a tree for IPv6. Each node in each tree constitutes a 0, or 1, in a
 * variable number of routing prefixes that are being stored in the tree. The
 * RIB data structure holds the root of each tree. Each left child node
 * represents a 0 bit in the routing prefix, whilst each right child node
 * represents a 1 bit in the prefix. These nodes are referred to as the node_t
 * (tree node) data structure. If a node marks the end of a routing prefix
 * (i.e. the final bit in the prefix) the complete_prefix bool variable is set,
 * and the Ordered Singly-Linked-List of nodes, constituting the
 * Routing-Table-Entries (RTEs), is added to. These nodes are referred to as
 * the Routing-Table-Entry (RTEs). The Singly-Linked-List of RTEs is ordered
 * based off the performance-metric (pm) of the associated interface (route
 * provided by interface).
 *
 *
 * Example RIB Construction:
 *                     -  RIB -
 *                   /     |    \
 *                  /      |     \
 *                 /       |      \
 *                /        |       \
 *               /         |        \
 *              /          |         \
 *       root_IPv4(/v6) size_IPv4   size_IPv6
 *          /        \
 *         0          1
 *       /   \      /   \
 *      0     1    0     1
 *               .
 *               .
 *               .
 *               .
 *               .
 *                \
 *39.228.247.71 <- 1 -> complete_prefix         => true
 *     ^                best_RTE       => RTE 1 => (gw = 27.3.3.1, ..., qual=10)
 *     |                next           => RTE 2 => (gw = 27.3.4.1, ..., qual=8)
 *     v                ...
 *routing-prefix        next           => RTE n => (gw = 28.3.5.1, ..., qual=1)
 *               .
 *               .
 *               .
 *               .
 *               .
 *               .
 *
 * NOTE: In the above example, the IP addresses in the IPv6 lookup tree are
 * replaced with IPv6 addresses for the gateway (gw), interface and routing
 * prefix.
 */

#endif //DATA_STRUCTURES_H
