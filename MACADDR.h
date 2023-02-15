#include <iostream>
#include <stdlib.h>
#include "logger.h"

using namespace std;

/* A mac address class */
class MACADDR
{
public:
  unsigned char mac[ETH_ALEN] = { 0 };
  MACADDR ()
  {
  }
  MACADDR (unsigned char a[ETH_ALEN])
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	mac[i] = a[i];
      }
  }

  void set (unsigned char a[ETH_ALEN])
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	mac[i] = a[i];
      }
  }
  bool operator< (const MACADDR a) const
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	if (a.mac[i] < this->mac[i])
	  return false;
	if (a.mac[i] > this->mac[i])
	  return true;
      }
    return false;
  }
  bool operator== (const MACADDR a) const
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	if (a.mac[i] != this->mac[i])
	  return false;
      }
    return true;
  }

  friend ostream & operator<< (std::ostream & os, const MACADDR & a)
  {
    os << hex;
    for (int i = 0; i < ETH_ALEN; i++)
      {
	os << (int) a.mac[i] << ' ';
      }
    os << dec;
    return os;
  }

  bool is_broadcast ()
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	if (mac[i] != 0xff)
	  return false;
      }
    return true;
  }

  bool is_multicast ()
  {

    if (mac[0] & 0x01)
      {
	return true;
      }
    return false;
  }

  void get_dest_mac (unsigned char *packet)
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	mac[i] = packet[i];
      }
  }

  void get_src_mac (unsigned char *packet)
  {
    for (int i = 0; i < ETH_ALEN; i++)
      {
	mac[i] = packet[i + ETH_ALEN];
      }
  }

  void random_mac ()
  {
    int i;
    for (i = 0; i < ETH_ALEN; i++)
      {
	mac[i] = rand () % 256;
      }
  }
};
