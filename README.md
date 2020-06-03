# VN-Exercise

### Resouces

[Longest-Prefix-Matching](https://www.geeksforgeeks.org/longest-prefix-matching-in-routers/) and 
[Routing Tables](https://www.geeksforgeeks.org/routing-tables-in-computer-network/) + some other websites on LPM/Routing tables


### Design

Designed as a static lib. IP lookup table is implemented as a tree, each node in the 
tree being a 0 or 1 of a routing prefix (a seperate tree for IPv4 and IPv6). Each prefix can have a variable number of 
Routing-Table-Entries (RTEs) containing the gateway (next-hop), the interface to forward on and the associated performance 
metric. The RTEs are maintained using an Ordered Singly-Linked-List, within the final bit nodes of routing prefixes. The 
head of the list contains the RTE with the best performance metric. A lot of functions contain just pseudo-code. 
`insert_RTE( char *addr, char *gw, char *intf, uint32_t qual)` is the only funciton of real importance that has been 
implemented.

### Improvements

Since this should be a library that would be availed of by multiple processes (BGP, RIP, OSPf....), it would be wise to turn it into a shared library (but wasn't for the sake of time... leaving out flock, mmap etc..). Since the vast majority is still pseudo-code, no testing suite has been added. As of yet, it's possible
to add as many RTE's to the SLL within a final bit prefix node. More than likely there are other edge cases I'm forgetting to
comment down here which I didn't account for.

### Different Approaches

Other possible approaches which were contemplated, were the following: 1) Implement a hash-map instead of a singly-linked-list 
=> (faster lookup if desiring the gateway (next-hop) if we already know the interface being forwarded on... by chance). 2) Use
a hash-map for IP-prefixes (Seperate chaining) (Probably a faster lookup of IP prefix)
