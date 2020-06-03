#ifndef IP_LOOKUP_H
#define IP_LOOKUP_H

#include <stdint.h>
#include <stdlib.h>

/*
 *  ------------------------------
 *  IP Lookup Table Static Library
 *  ------------------------------
 *
 *  This library contains basic functionality to create/destroy an IP lookup
 *  table/Routing-Information-Base (RIB), to insert/remove Routing-Table-Entries
 *  (RTEs) into/from it and to find the best interface to forward packets on for
 *  a specific destination IP address.
 *  The Library takes both IPv4 and IPv6 addresses in the form of strings
 *  (char *). The format of ipv4 can include a CIDR mask or not, for example:
 *
 *  valid IPv4 address with mask: "1.2.3.4/20"
 *  valid IPv4 address without mask: "1.2.3.4"
 *
 *  valid IPv6 address with mask: "::1001:1010/22"
 *  valid IPv6 address without mask: "::1001:1010"
 */

/*
 * Public API Function Declarations
 */

/**
 * @function init_table
 * @description Initialise the IP lookup table for inserting, deleting and
 * finding the RTEs when passed an IP address.
 */
void init_table( void );

/**
 * @function cleanup
 * @description Tear down procedure, freeing memory used by the IP table.
 */
void cleanup( void );

/**
 * @function insert_RTE
 * @description Insert RTE into the table.
 *
 * @param addr: (IPv4 | IPv6) address (CIDR notation).
 * @param gw:   (Gateway) Address of next-hop to be taken.
 * @param intf: Address of interface to forward packets on.
 * @param qual: Superficial QOS/pm numeric value of the associated
 *              interface to forward packets on to reach the associated
 *              network.
 *
 * @return 0  => If successfully inserted,
 *         -1 => Otherwise.
 */
uint8_t insert_RTE( char *addr, char *gw, char *intf, uint32_t qual );

/**
 * @function find_best_intf
 * @description Find the best interface to forward on, given the desired
 * destination address, using Longest-Prefix-Matching (LPM).
 *
 * @param dest: (IPv4 | IPv6) destination address.
 *
 * @return Best interface to forward on => If appropriate RTE exists,
 *         NULL                         => Otherwise.
 */
char *find_best_intf( char *dest );

/**
 * @function remove_RTE
 * @description Delete RTE in the table.
 *
 * @param addr: (IPv4 | IPv6) address (CIDR notation).
 * @param gw:   (Gateway) Address of next-hop.
 *
 * @return 0  => If successfully removed,
 *         -1 => Otherwise.
 */
uint8_t remove_RTE( char *addr, char *gw );

/**
 * @function num_of_IPv4_RTEs
 * @description Retrieves the number of IPv4 RTEs
 *
 * @return number of IPv4 RTEs
 */
size_t num_of_IPv4_RTEs();

/**
 * @function num_of_IPv6_RTEs
 * @description Retrieves the number of IPv6 RTEs
 *
 * @return number of IPv6 RTEs
 */
size_t num_of_IPv6_RTEs();

#endif //IP_LOOKUP_H
