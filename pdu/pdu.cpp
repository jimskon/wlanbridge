
#include "pdu.h"
#include "pdu_eth.h"
#include <arpa/inet.h>
#include <iostream>
#include <cstring>

static const char* pkttypes[] = {
	"HOST",      // PACKET_HOST
	"BROADCAST", // PACKET_BROADCAST
	"MULTICAST", // PACKET_MULTICAST
	"OTHERHOST", // PACKET_OTHERHOST
	"OUTGOING",  // PACKET_OUTGOING
	"LOOPBACK",  // PACKET_LOOPBACK
	"USER",      // PACKET_USER
	"KERNEL"     // PACKET_KERNEL
};

//  - - - - - - - - -  //
//  S T R U C T O R S  //
//  - - - - - - - - -  //

pdu::pdu( uint32_t cap )
{
        // Leave room for vlan tag
        _off = 8;
	_buf = new uint8_t[ cap+16 ];
	_x = _buf+8;
	_cap = cap;
	_len = 0;
	
}


/*pdu::pdu( uint8_t *x, uint32_t len )
{
         // Leave room for vlan tag
        _off = 8;
        _buf = new uint8_t[ len+16 ];
        _x = _buf+8;
        _cap = len;
        _len = len;
	memcpy(_x,x,len);


	}*/
pdu::pdu( uint8_t *x, uint32_t len )
        : _x( x ), _buf(x), _cap( 0 ), _len( len ), _off( 0 )
{ ; }

pdu::~pdu()
{
	if( _cap > 0 )
		delete[] _buf;
}

//  - - - - - - -  //
//  M E T H O D S  //
//  - - - - - - -  //

int
pdu::filter( std::ostream& os )
{
	pdu_eth eth( *this, 0 );

	os << "pkttype: "  << (int)_sll.sll_pkttype
	                   << " (" << pkttypes[_sll.sll_pkttype & 0x07] << ')'
	   << ", length: " << _len
	   << std::endl;

	return
		eth.filter( os );
}


#define VLAN_PROTO 0x8100

int pdu::vlan_untag() {

  struct ethhdr_v *eth = (struct ethhdr_v*)_x;
  uint16_t proto = ntohs( eth->proto );
  int16_t vlan = -1;   /* Default to no vlan tag */
  if (proto == VLAN_PROTO) {
    vlan = ntohs(eth->VLANTag);

    /* Remove tag */
    uint8_t *ptr = (uint8_t *) eth;
    for (int i=11; i>=0; i--) {
      ptr[i+4] = ptr[i];
    }
    _x += 4;
    _off += 4;
    _len-=4;
  }
  
  return vlan;
}

void pdu::vlan_tag(uint32_t tag) {
  if (_off < 8) {
    std::cout << "VLAN tag error - no space in packet!" << _off << ":" << _len<< ":" << _cap << std::endl;
  }
  uint16_t _tag = (uint16_t) tag;
  _off -=4 ;
  _x -= 4;
  _len+=4;
  struct ethhdr_v *eth = (struct ethhdr_v*)_x;
  /* add tag space */
  uint8_t *ptr = (uint8_t *) eth;
  for (int i=0; i<12; i++) {
    ptr[i] = ptr[i+4];
  }
  eth->VLANTag=htons(_tag);
  eth->proto=htons(VLAN_PROTO);
  
}



/*EoF*/
