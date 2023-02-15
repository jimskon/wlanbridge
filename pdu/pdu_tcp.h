
#ifndef _UBNG_PDU_TCP_H
#define _UBNG_PDU_TCP_H

#include "pdu.h"

struct pdu_tcp final : public pdu
{
	pdu_tcp( uint8_t *x, size_t cap ) : pdu( x, cap )
	{ ; }

	pdu_tcp( const pdu& other, size_t off )
		: pdu( other._x + off, other._len - off )
	{ ; }

	virtual ~pdu_tcp()
	{ ; }

	int filter( std::ostream& log ) override;
	int adjust_mss( uint8_t* x, size_t len );
};

#endif /*_UBNG_PDU_TCP_H*/

/*EoF*/
