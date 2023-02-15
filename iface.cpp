
#include <cstdio>
#include <cstring>
#include <cassert>

#include <sys/socket.h>
#include <sys/ioctl.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include "brmap.h"
#include "iface.h"

//  - - - - - - - - -  //
//  S T R U C T O R S  //
//  - - - - - - - - -  //

iface::iface()
{
	(void)::memset( _hwaddr, 0, sizeof( _hwaddr ));
	_name  = "(null)";
	_index =  0;
	_sock  = -1;
	_mtu   =  0;
}

iface::~iface()
{
	(void)promisc( false );
	(void)::close( _sock );
}

//  - - - - - - -  //
//  M E T H O D S  //
//  - - - - - - -  //

int
iface::bind( const char *name )
{
	struct sockaddr_ll sll;
	struct ifreq req;

	_name = name;

	_sock = ::socket( AF_PACKET, SOCK_RAW, ::htons( ETH_P_ALL ));
	if( _sock < 0 ) {
		::perror( _name );
		return -1;
	}

	/* store the interface L2 address in _hwaddr (only used for debugging) */

	(void)::strncpy( req.ifr_name, _name, IFNAMSIZ-1 );
	if( ::ioctl( _sock, SIOCGIFHWADDR, &req ) < 0 ) {
		::perror( _name );
		(void)::close( _sock );
		return -1;
	}

	(void)::memcpy( _hwaddr, req.ifr_hwaddr.sa_data, ETH_ALEN );

	/* store the interface index in _index */

	if( ::ioctl( _sock, SIOCGIFINDEX, &req ) < 0 ) {
		::perror( _name );
		(void)::close( _sock );
		return -1;
	}

	_index = req.ifr_ifindex;

	/* store the interface MTU in _mtu */

	if( ::ioctl( _sock, SIOCGIFMTU, &req ) < 0 ) {
		::perror( _name );
		(void)::close( _sock );
		return -1;
	}

	_mtu = req.ifr_mtu;

	/*
	 * populate a sockaddr_ll instance and bind the interface to it
	 * (might set sll_protocol here as well -- see note in iface::recv())
	 */

	(void)::memset( &sll, 0, sizeof( sll ));
	sll.sll_family  = AF_PACKET; /* only interested in packets from the AF_PACKET family */
	sll.sll_ifindex = _index;    /* only interested in packets captured from this interface */

	if( ::bind( _sock, (const struct sockaddr*)&sll, sizeof( sll )) < 0 ) {
		::perror( _name );
		(void)::close( _sock );
		return -1;
	}

	return 0;
}

int
iface::promisc( bool enable )
{
	struct packet_mreq mr;
	int    op;

	(void)::memset( &mr, 0, sizeof( mr ));
	mr.mr_ifindex = _index;
	mr.mr_type    = PACKET_MR_PROMISC;

	op = enable ? PACKET_ADD_MEMBERSHIP : PACKET_DROP_MEMBERSHIP;

	return
		::setsockopt( _sock, SOL_PACKET, op, &mr, sizeof( mr ));
}

/*
void
iface::cleanup()
{
	(void)promisc( false );
	(void)::close( ifp->_sock );
}
*/

int
iface::recv( struct pdu& pkt )
{
	socklen_t fromlen;
	int n;

	struct sockaddr_ll& sll = pkt._sll;

	fromlen = sizeof( sll );
	n = ::recvfrom( _sock, pkt._x, pkt._cap, 0, (struct sockaddr*)&sll, &fromlen );

	if( n < 0 )
		return n;

	pkt._len = n;

	/* ignore packets involving our host -- see PACKET(7) */

	switch( sll.sll_pkttype ) {
		case PACKET_HOST:      /* packet is addressed to us */
		case PACKET_OUTGOING:  /* packet originated from us */
			return 0;

		default:
			;
	}

	/* something's wrong with the interface setup if any of these fail */

	assert( sll.sll_family  == AF_PACKET );
	assert( sll.sll_ifindex == _index );

	return n;
}

int
iface::send( struct pdu& pkt )
{
	struct ethhdr *eh;

	struct sockaddr_ll& sll = pkt._sll;

	/*
	 * Quoting PACKET(7):
	 * "When you send packets, it is enough to specify sll_family, sll_addr,
	 *  sll_halen, sll_ifindex, and sll_protocol. The other fields should be 0."
	 */

	eh = (struct ethhdr*)pkt._x;

	(void)::memset( &sll, 0, sizeof( sll ));
	(void)::memcpy( sll.sll_addr, eh->h_source, ETH_ALEN );
	sll.sll_halen    = ETH_ALEN;
	sll.sll_family   = AF_PACKET;
	sll.sll_ifindex  = _index;
	sll.sll_protocol = eh->h_proto;

	return
		::sendto( _sock, pkt._x, pkt._len, 0, (struct sockaddr*)&sll, sizeof( sll ));
}

//  - - - - - -  //
//  S T A T I C  //
//  - - - - - -  //

std::ostream& operator<< ( std::ostream& os, const iface& obj )
{
	char straddr[12+5+1];

	const uint8_t *x = obj._hwaddr;
	(void)::snprintf( straddr, sizeof( straddr ),
		"%02x:%02x:%02x:%02x:%02x:%02x",
		x[0], x[1], x[2], x[3], x[4], x[5] );

	os << obj._name << ": "
	   << "index: "    << obj._index
	   << ", mtu: "    << obj._mtu
	   << ", hwaddr: " << straddr
	;

	return os;
}

/*EoF*/
