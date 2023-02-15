
#ifndef _UBNG_PDU_UDP_H
#define _UBNG_PDU_UDP_H

#include "pdu.h"

struct pdu_udp final : public pdu
{
	pdu_udp( uint8_t *x, size_t cap ) : pdu( x, cap )
	{ ; }

	pdu_udp( const pdu& other, size_t off )
		: pdu( other._x + off, other._len - off )
	{ ; }

	virtual ~pdu_udp()
	{ ; }

	int filter( std::ostream& log ) override;
};

#endif /*_UBNG_PDU_UDP_H*/

/*EoF*/
