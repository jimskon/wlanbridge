
#ifndef _UBNG_PDU_ETH_H
#define _UBNG_PDU_ETH_H

#include "pdu.h"
// 0       6      12      12/16 14/18           18/22                                                  
// +-------+-------+---------+----+---------------+                                                    
// | DMAC  | SMAC  |8100 VLAN|Type|Payload (4Bfix)|                                                    
// +-------+-------+---------+----+---------------+                                                    
//                  <-------> when VLAN == Yes      
struct ethhdr_v {
  uint8_t dest[6];
  uint8_t src[6];
  uint16_t proto;
  uint16_t VLANTag;
  uint16_t type;
  int32_t  payload;
} __attribute__((packed));

struct pdu_eth final : public pdu
{
	pdu_eth( uint8_t *x, size_t len ) : pdu( x, len )
	{ ; }

	pdu_eth( const pdu& other, size_t off )
		: pdu( other._x + off, other._len - off )
	{ ; }

	virtual ~pdu_eth()
	{ ; }

	int filter( std::ostream& log ) override;

	// int vlan_untag() override;  /* Returns vlan tag value, -1 if no tag */

	// void vlan_tag(uint32_t tag) override; /* Add vlan tag */

	
};

std::ostream& operator<< ( std::ostream& os, const struct ethhdr* eth );

#endif /*_UBNG_PDU_ETH_H*/

/*EoF*/
