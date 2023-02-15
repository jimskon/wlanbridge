
#ifndef _UBNG_PDU_IPv4_H
#define _UBNG_PDU_IPv4_H

#include "pdu.h"

struct pdu_ipv4 final : public pdu
{
	pdu_ipv4( uint8_t *x, size_t cap ) : pdu( x, cap )
	{ ; }

	pdu_ipv4( const pdu& other, size_t off )
		: pdu( other._x + off, other._len - off )
	{ ; }

	virtual ~pdu_ipv4()
	{ ; }

	int filter( std::ostream& log ) override;
};

#endif /*_UBNG_PDU_IPv4_H*/

/*EoF*/
