
#include <cstdio>
#include <cstring>

#include <net/ethernet.h>
#include <arpa/inet.h>

#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include "icmp4.h"

static int icmp4_gen_needfrag_eth  ( uint8_t *, int, const struct ethhdr*, int );
static int icmp4_gen_needfrag_ip   ( uint8_t *, int, const struct iphdr*,  int );
static int icmp4_gen_needfrag_icmp ( uint8_t *, int, const struct iphdr*,  int );

static uint16_t
in_cksum( const void *addr, int len )
{
	const uint16_t *w = (const uint16_t*)addr;
	unsigned sum = 0;

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

static int
icmp4_gen_needfrag_eth( uint8_t *x, int xlen, const struct ethhdr *seh, int mtu )
{
	const struct iphdr *sih;
	struct ethhdr *deh;
	int len, dlen;

	sih  = (const struct iphdr*)((const uint8_t*)seh + ETH_HLEN);
	dlen = icmp4_gen_needfrag_ip( x + ETH_HLEN, xlen - ETH_HLEN, sih, mtu );
	deh  = (struct ethhdr*)x;
	len  = ETH_HLEN + dlen;

	(void)memcpy( deh->h_dest, seh->h_source, ETH_ALEN );
	(void)memcpy( deh->h_source, seh->h_dest, ETH_ALEN );
	deh->h_proto = htons( ETH_P_IP );

	return len;
}

static int
icmp4_gen_needfrag_ip( uint8_t *x, int xlen, const struct iphdr *sih, int mtu )
{
	struct iphdr *dih;
	int len, dlen;

	dlen = icmp4_gen_needfrag_icmp( x + 20, xlen - 20, sih, mtu );
	dih  = (struct iphdr*)x;
	len  = 20 + dlen;

	dih->ihl      = 5;
	dih->version  = 4;
	dih->tos      = IPTOS_CLASS_DEFAULT;
	dih->tot_len  = htons( len );
	dih->id       = 0;
	dih->frag_off = 0;
	dih->ttl      = IPDEFTTL;
	dih->protocol = IPPROTO_ICMP;
	dih->check    = 0;
	dih->saddr    = sih->daddr;
	dih->daddr    = sih->saddr;

	dih->check = in_cksum( x, 20 );

	return len;
}

static int
icmp4_gen_needfrag_icmp( uint8_t *x, int xlen, const struct iphdr *sih, int mtu )
{
	struct icmphdr *dich;
	int len, dlen;

	(void)xlen;
	dich = (struct icmphdr*)x;

	dich->type        = ICMP_DEST_UNREACH;
	dich->code        = ICMP_FRAG_NEEDED;
	dich->checksum    = 0;
	dich->un.frag.mtu = htons( (uint16_t)( mtu & 0xffff ));

	dlen = ( sih->ihl << 2 ) + 8;
	len  = ICMP_HLEN + dlen;

	(void)memcpy( x + ICMP_HLEN, sih, dlen );
	dich->checksum = in_cksum( x, len );

	return len;
}

int
icmp4_gen_needfrag( uint8_t *x, int xlen, const uint8_t *pkt, int mtu )
{
	return
		icmp4_gen_needfrag_eth( x, xlen, (const struct ethhdr*)pkt, mtu );
}

/*EoF*/
