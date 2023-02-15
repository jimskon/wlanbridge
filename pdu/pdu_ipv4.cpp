
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "pdu_ipv4.h"
#include "pdu_udp.h"
#include "pdu_tcp.h"

static const char*
ipv4_proto_str( unsigned proto )
{
	switch( proto ) {
		case IPPROTO_ICMP: return " (ICMP)";
		case IPPROTO_IGMP: return " (IGMP)";
		case IPPROTO_UDP:  return " (UDP)";
		case IPPROTO_TCP:  return " (TCP)";
		default:
			;
	}

	return "";
}

static uint16_t
in_cksum( const void *addr, int len, unsigned sum = 0 )
{
	const uint16_t *w = (const uint16_t*)addr;

	while( len > 1 ) {
		sum += *w++;
		len -= 2;
	}

	if( len == 1 ) {
		uint16_t odd = 0;
		*(uint8_t*)(&odd) = *(const uint8_t*)w ;
		sum += odd;
	}

	sum  = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);

	return( ~sum );
}

struct pseudohdr
{
	uint32_t saddr;
	uint32_t daddr;
	uint8_t  zero;
	uint8_t  proto;
	uint16_t len;
};

//  - - - - - -  //
//  P U B L I C  //
//  - - - - - -  //

int
pdu_ipv4::filter( std::ostream& log )
{
	struct iphdr *ih = (struct iphdr*)_x;
	size_t off = ( ih->ihl << 2 );

	struct in_addr saddr = { ih->saddr };
	struct in_addr daddr = { ih->daddr };

	log << " > IPv4: "
	    << inet_ntoa( saddr )
	    << " -> "
	    << inet_ntoa( daddr )
	    << ", id: "     << ntohs( ih->id )
	    << ", ttl: "    << (unsigned)ih->ttl
	    << ", proto: "  << (unsigned)ih->protocol
	                    << ipv4_proto_str( ih->protocol )
	    << ", length: " << _len
	    << std::endl;

	switch( ih->protocol ) {
		case IPPROTO_TCP:
		{
			pdu_tcp tcp( *this, off );
			if( tcp.filter( log )) {
				/*
				 * pdu_tcp was modified, we need to recompute checksums.
				 * First, do TCP:
				 */
				struct tcphdr   *th = (struct tcphdr*)tcp._x;
				struct pseudohdr ph = {
					ih->saddr,
					ih->daddr,
					0,
					ih->protocol,
					htons( (uint16_t)((_len-off) & 0xffff))
				};
				const uint16_t *w = (const uint16_t*)&ph;
				unsigned sum = 0;
				for( int i = 0; i < 6; i++ )
					sum += w[i];
				th->th_sum = 0;
				th->th_sum = in_cksum( _x+off, _len-off, sum );
				/*
				 * Then do IP:
				 */
				ih->check = 0;
				ih->check = in_cksum( _x, off );
				//(void)fprintf( stderr, "*** CHECKSUMS: TCP: %04x, IP: %04x\n", ntohs( th->th_sum ), ntohs( ih->check ));
			}

			break;
		}

		case IPPROTO_UDP:
		{
			pdu_udp udp( *this, off );
			return udp.filter( log );
		}

		default:
			;
	}

	return 1;
}

/*EoF*/
