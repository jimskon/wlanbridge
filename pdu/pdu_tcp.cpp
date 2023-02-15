
#include <iostream>
#include <cstring>

#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pdu_tcp.h"

static const char *tcp_flags[6] = { "URG", "ACK", "PSH", "RST", "SYN", "FIN" };

static void
tcp_flags_dump( std::ostream& log, unsigned flags )
{
	int i, n;
	unsigned mask = 0x20;

	for( i = 0, n = 0; i < 6; i++ ) {
		if( flags & mask ) {
			if( n++ > 0 ) log << ',';
			log << tcp_flags[i];
		}
		mask >>= 1;
	}
}

//  - - - - - -  //
//  P U B L I C  //
//  - - - - - -  //

int
pdu_tcp::filter( std::ostream& log )
{
	struct tcphdr *th = (struct tcphdr*)_x;
	size_t off = ( th->th_off << 2);

	log << " > TCP: "
	    << ntohs( th->th_sport )
	    << " -> "
	    << ntohs( th->th_dport )
	    << ", off: " << off
	    << ", flags: <"
	;

	tcp_flags_dump( log, th->th_flags );

	log << '>' << std::endl;

	if(( th->th_flags & TH_SYN ) == 0 )
		return 0; /* not a SYN ? don't bother */

	if( off == 20 )
		return 0; /* no TCP options ? don't bother */

	return
		adjust_mss( _x + 20, off - 20 );
}

int
pdu_tcp::adjust_mss( uint8_t *x, size_t len )
{
	size_t   olen;
	uint16_t mss;
	unsigned opt;

	int      mod = 0;
	uint8_t *end = x + len;

	while( x < end ) {
		opt = x[0];

		if( opt == TCPOPT_EOL ) {
			break;
		}

		if( opt == TCPOPT_NOP ) {
			x++; continue;
		}

		olen = x[1];
		if(( x + olen ) > end ) {
			// malformed TCP segment
			break;
		}

		if( opt == TCPOPT_MAXSEG ) {
			if( olen != TCPOLEN_MAXSEG ) {
				// shouldn't happen
				break;
			}

			(void)::memcpy( &mss, x+2, sizeof( mss ));
			// adjust to 1400 for now
			if( ntohs( mss ) > 1400 ) {
				mss = htons( 1400 );
				(void)::memcpy( x+2, &mss, sizeof( mss ));
				mod = 1;
			}
		}

		x += olen;
	}

	return mod;
}

/*EoF*/
