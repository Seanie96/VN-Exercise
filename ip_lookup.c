#include "ip_lookup.h"
#include "data_structures.h"
#include <stdio.h>
#include <bits/types/FILE.h>

#define RIB_SAVE_LOCATION "/var/iptable/ip_lookup_table"
#define DEFAULT_GW_IPV4 "24.0.24.24"
#define DEFAULT_GW_IPV6 "1234:1234:1234:1234:1234:1234:1234:1234"
#define DEFAULT_INTF_IPV4 "123.123.0.123"
#define DEFAULT_INTF_IPV6 "11:11:11:11:11::11"

// IP Lookup Tree Data Structure Instance
static RIB *rib = NULL;
static bool already_cleaned_up = false;

/*
 * Static Utility Functions
 * I have left most of these unimplemented, or given them pseudo-code....
 * I have also excluded descriptions of these functions since they are static,
 * and their names give an adequate description
 */

static __uint128_t addr_to_bin( char *addr, uint8_t *is_IPv6, uint16_t *mask ) {
    /*
     * 1) Check whether it's in a valid IPv4/v6 address (CIDR) format
     * 2) Convert string address to decimal representation
     *
     * NOTE: If IPv4 address, then 96 msb's of the first 128 bits are 0,
     * and address starts at the 32 bit
     *
     * Example Input/Output:
     *      addr = "::1111/127", returns 4369, *is_IPv6 = 1, *mask = 127
     *      addr = "0.0.1.1/28", returns 257, *is_IPv6 = 0, *mask = 28
     *
     * Could be possible to turn this into a Macro...
     */
    return -1;
}

static void serialise_data() {
    /*
     * Convert pointers to an array data structure representation so that
     * it can be written to a file
     */
}

static RIB *deserialise_data() {
    /*
     * Convert array representation of rib, within the rib save file, back to a
     * RIB data structure
     */
    return NULL;
}

static void free_memory() {
    /*
     * DFS traversal of tree nodes, riding the Ordered SLL nodes within tail
     * tree nodes to free them, and then eventually freeing the tail tree node
     * itself before moving back up the tree to repeat the process
     */
    return;
}

static node_t *new_node_t() {
    node_t *new_node = ( node_t * )malloc( sizeof( node_t ) );

    new_node->one = NULL;
    new_node->zero = NULL;
    new_node->best_RTE = NULL;
    new_node->complete_prefix = false;
    new_node->size = 0;

    return new_node;
}

static RTE *new_RTE( char *gw, char *intf, uint32_t qual ) {
    RTE *new_rte = ( RTE * )malloc( sizeof( RTE ) );

    new_rte->next = NULL;
    new_rte->gw = gw;
    new_rte->intf = intf;
    new_rte->qual = qual;

    return new_rte;
}

/*
 * API Function Implementations
 */

void cleanup( void ) {
    if ( !already_cleaned_up ) {
        // Save RIB.
        serialise_data();

        // Free memory allocated to tree.
        free_memory();

        already_cleaned_up = true;
    }
}

void init_table( void ) {
    FILE *saved_rib;
    if ( ( saved_rib = fopen( RIB_SAVE_LOCATION, "r" ) ) ) {
        // Attempt to restore a saved IP routing table
        rib = deserialise_data( saved_rib );
    } else  {
        // Create a new IP routing table
        rib = ( RIB * )malloc( sizeof( RIB ) );

        rib->root_IPv4 = new_node_t();
        rib->size_IPv4 = 0;

        rib->root_IPv6 = new_node_t();
        rib->size_IPv6 = 0;
    }
    // Register the cleanup function, in the scenario where the user forgets to
    // call...
    atexit( cleanup );
}

uint8_t insert_RTE( char *addr, char *gw, char *intf, uint32_t qual ) {
    if ( !rib ) {
        return -1;
    }
    // Convert string address to binary version and attain mask
    uint8_t *is_IPv6 = ( uint8_t * )malloc( sizeof( uint8_t ) );
    uint16_t *mask = ( uint16_t * )malloc( sizeof( uint16_t ) );
    __uint128_t addr_bin = addr_to_bin( addr, is_IPv6, mask );
    node_t *current_node = rib->root_IPv6;
    size_t *size = &rib->size_IPv6;
    uint16_t bit_length = 128;
    if ( !*is_IPv6 ) {
        current_node = rib->root_IPv4;
        size = &rib->size_IPv4;
        bit_length = 32;
    }

    // Create path through the tree for routing prefix, if the path does not
    // exist already, and insert the RTE into the Ordered SLL of RTEs for that
    // tail node i.e. that routing prefix, at the correct position. At the front
    // if it has the best performance-metric quality
    uint16_t stopping_index = bit_length - *mask;
    for ( uint16_t i = ( bit_length - 1 ); i >= stopping_index; --i ) {
        // Isolate bit at index i of addr_bin
        uint8_t desired_bit = ( addr_bin >> i ) & 1;
        if ( desired_bit ) {
            if ( !current_node->one ) {
                // Create right child, bit = 1, since it doesn't already exist
                current_node->one = new_node_t();
            }
            current_node = current_node->one;
        } else  {
            if ( !current_node->zero ) {
                // Create left child, bit = 0, since it doesn't already exist
                current_node->zero = new_node_t();
            }
            current_node = current_node->zero;
        }

        // Create new RTE and find position to place in Ordered SLL, since this
        // is the last bit of the routing prefix of addr_bin. This
        // implementation does not limit the number of RTE's that can be added
        // linked list, which would need to happen in reality...
        if ( i == stopping_index ) {
            current_node->complete_prefix = true;
            // Create new RTE
            RTE *new_rte = new_RTE( gw, intf, qual );

            // Find correct position
            RTE *current_RTE = current_node->best_RTE;
            RTE *previous_RTE = NULL;
            while ( current_RTE && ( current_RTE->qual > new_rte->qual ) ) {
                previous_RTE = current_RTE;
                current_RTE = current_RTE->next;
            }

            if ( !previous_RTE ) {
                new_rte->next = current_RTE;
                current_node->best_RTE = new_rte;
            } else  {
                previous_RTE->next = new_rte;
                new_rte->next = current_RTE;
            }
        }
    }

    *size = *size + 1;
    free( size );
    free( is_IPv6 );
    free( current_node );
    free( mask );
    return 0;
}

char *find_best_intf( char *dest ) {
    /*
     * 1) Check to see if dest/rib are NULL, if one is, return NULL
     * 2) Call addr_to_bin() to check whether the destination address is a valid
     *    address and to convert to a decimal representation
     * 3) LPM search pseudo-code
     *      3.1) If the ith bit from dest, starting from either the 128th/
     *           32nd bit, is one, take the 'one' child tree node, otherwise the
     *           'zero' child tree node. If the child node marks the end of a
     *           prefix i.e. complete_prefix = true, then save the best
     *           interface in the RTEs (the RTE at the head of the SLL)
     *      3.2) Continue to the next bit in the destination address and to the
     *           next child node. If a child node is NULL before the final bit
     *           of the passed destination address is met, return either the
     *           saved interface or the default DEFAULT_INTF_IPV4/V6 macro
     *           constant interface if no interface was saved i.e. no saved
     *           routing prefixes matching the destination address
     */
    return NULL;
}

uint8_t remove_RTE( char *addr, char *gw ) {
    /*
     * 1) Check to see if addr/gw/rib are NULL, if one is, return -1
     * 2) Call addr_to_bin() to check whether the address is a valid
     *    address and to convert it to a decimal representation
     * 3) Search for routing prefix of the passed address by walking the tree
     *      4.1) If final bit of prefix of addr exists in tree, remove the RTE
     *           with the same gateway value as gw, and return 0
     *      4.2) If it does not exist in the tree, return 0 anyway? As it does
     *           not exist in the tree to remove...
     */
    return -1;
}

size_t num_of_IPv4_RTEs() {
    if ( rib ) {
        return rib->size_IPv4;
    } else  {
        return -1;
    }
}

size_t num_of_IPv6_RTEs() {
    if ( rib ) {
        return rib->size_IPv6;
    } else  {
        return -1;
    }
}
