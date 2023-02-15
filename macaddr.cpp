
#include <cstdio>

#include "macaddr.h"

//  - - - - - - - - -  //
//  S T R U C T O R S  //
//  - - - - - - - - -  //

macaddr::macaddr( const uint8_t* x ) : _x( x )
{ ; }

//  - - - - - - -  //
//  O S T R E A M  //
//  - - - - - - -  //

std::ostream&
operator<< ( std::ostream& os, const macaddr& mac )
{
	char straddr[12+5+1];

	(void)snprintf( straddr, sizeof( straddr ),
		"%02x:%02x:%02x:%02x:%02x:%02x",
		mac._x[0], mac._x[1], mac._x[2],
		mac._x[3], mac._x[4], mac._x[5]
	);

	os << straddr;

	return os;
}

/*EoF*/
