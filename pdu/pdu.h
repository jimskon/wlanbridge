
#ifndef _UBNG_PDU_H
#define _UBNG_PDU_H

#include <cstdlib>
#include <cstdint>
#include <ostream>

#include <linux/if_packet.h>

struct pdu
{
	uint8_t *_x;   // buffer (current)
        uint8_t *_buf; // buffer (complete) 
	size_t   _cap; // capacity
	uint32_t   _len; // length
	uint32_t   _off; // offset in buffer

	struct sockaddr_ll _sll; // link-layer info

	pdu( uint32_t cap );
        pdu( uint8_t *x, uint32_t cap );
	virtual ~pdu();

	virtual int filter( std::ostream& log );
        virtual int vlan_untag();
        virtual void vlan_tag(uint32_t tag);
};

#endif /*_UBNG_PDU_H*/

/*EoF*/
