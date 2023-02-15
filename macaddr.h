
#ifndef _UBNG_MACADDR_H
#define _UBNG_MACADDR_H

#include <cstdint>
#include <ostream>

#include <net/ethernet.h>

class macaddr
{
	private:

		constexpr static uint8_t _zero[ETH_ALEN] = { 0, 0, 0, 0, 0, 0 };

		const uint8_t *_x;

	public:

		macaddr( const uint8_t *x = _zero );

		friend std::ostream& operator<< ( std::ostream& os, const macaddr& mac );
};

#endif /*_UBNG_MACADDR_H*/

/*EoF*/
