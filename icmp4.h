
#ifndef _UBNG_ICMP4_H
#define _UBNG_ICMP4_H

#include <stdint.h>

#ifndef ICMP_HLEN
 #define ICMP_HLEN 8
#endif

extern int icmp4_gen_needfrag( uint8_t *x, int xlen, const uint8_t *pkt, int mtu );

#endif /*_UBNG_ICMP4_H*/

/*EoF*/
