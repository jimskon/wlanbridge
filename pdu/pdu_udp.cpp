
#include <netinet/udp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pdu_udp.h"

//  - - - - - -  //
//  P U B L I C  //
//  - - - - - -  //

int
pdu_udp::filter( std::ostream& log )
{
	struct udphdr *uh = (struct udphdr*)_x;

	log << " > UDP: "
	    << ntohs( uh->uh_sport )
	    << " -> "
	    << ntohs( uh->uh_dport )
	    << ", length: " << _len
	    << std::endl;

	return 1;
}

/*EoF*/
