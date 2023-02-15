
#include <cstdio>
#include <iostream>
#include <arpa/inet.h>

#include "pdu_eth.h"
#include "pdu_ipv4.h"
#include "macaddr.h"

//  - - - - - -  //
//  P U B L I C  //
//  - - - - - -  //

int
pdu_eth::filter( std::ostream& log )
{
	char strproto[8];

	struct ethhdr *eth = (struct ethhdr*)_x;
	uint16_t proto = ntohs( eth->h_proto );
	(void)snprintf( strproto, sizeof( strproto ), "0x%04x", proto );

	log << " > ETH: "
	    << macaddr( eth->h_source )
	    << " -> "
	    << macaddr( eth->h_dest )
	    << ", proto: "  << strproto
	    << ", length: " << _len
	    << std::endl;
	;

	switch( proto ) {
		case ETHERTYPE_IP:
		{
			pdu_ipv4 ipv4( *this, ETH_HLEN );
			return ipv4.filter( log );
		}

/*
		case ETHERTYPE_IPv6:
		{
			return 0;
		}
*/

		default:
			;
	}

	return 1;
}

/* 
std::ostream&
pdu_eth::dump( std::ostream& os ) const
{
	char strproto[8];

	struct ethhdr *eth = (struct ethhdr*)_x;
	uint16_t proto = ntohs( eth->h_proto );
	(void)snprintf( strproto, sizeof( strproto ), "0x%04x", proto );

	os << "ETH: "
	   << macaddr( eth->h_source )
	   << " -> "
	   << macaddr( eth->h_dest )
	   << ", proto: " << strproto
	;

	switch( proto ) {
		case ETHERTYPE_ARP:    os << " (ARP)";    break;
		case ETHERTYPE_REVARP: os << " (REVARP)"; break;
		case ETHERTYPE_IP:     os << " (IPv4)";   break;
		case ETHERTYPE_IPV6:   os << " (IPv6)";   break;
		case ETHERTYPE_VLAN:   os << " (VLAN)";   break;
		case 0x88a8:           os << " (QinQ)";   break;
		case 0x88cc:           os << " (LLDP)";   break;

		default:
			if( proto <= 0x5dc ) {
				os << " (802.3)";
			} else {
				os << " (unknown)";
			}
	}

	os << ", length: " << _len;

	return os;
}
*/

/*EoF*/
