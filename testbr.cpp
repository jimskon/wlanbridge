
#include <iostream>
#include "brmap.h"

void printpacket(const char* msg, const unsigned char* p, size_t len) {
    size_t i;
    cout << msg << " len=" << len << endl;
    for(i=0; i<len; ++i) {
      cout << std::hex << (int) p[i];
    }
    cout << std::dec << endl;
}


int main() {
    srand(time(NULL) + getpid());
    unsigned char pkt1[] = {0x42,0x56,0x34,0x45,0x23,0x56,0x32,0x56,0x67,0x32,0x56,0x34,0x34,0x76,0x34,0x04,0x23,0x13,0x75,0x76};  
    unsigned char pkt2[] = {0x32,0x56,0x67,0x32,0x56,0x34,0x42,0x56,0x34,0x45,0x23,0x56,0x56,0x76,0x34,0x04,0x23,0x13,0x75,0x76};  
    unsigned char pkt3[] = {0x58,0x55,0x55,0x34,0x56,0x34,0x32,0x56,0x67,0x32,0x56,0x34,0x58,0x55,0x55,0x34,0x56,0x34,0x58,0x56,0x34,0x45,0x23,0x56,0x56,0x76,0x34,0x04,0x23,0x13,0x75,0x76};  
    unsigned char pkt4[] = {0xAA,0x55,0x55,0x34,0x56,0x34,0xAC,0x56,0x34,0x45,0x23,0x56,0x56,0x76,0x34,0x04,0x23,0x13,0x75,0x76};
    unsigned char pkt[128] = {0};

    logger log;
    MACADDR a,b;
    brmap bridge( log );
    
    log.level(1);
    //    bridge.map_pkt(1,pkt1);
    //bridge.map_pkt(2,pkt2);
    //bridge.map_pkt(3,pkt3);
    //bridge.map_pkt(4,pkt4);
    //bridge.print();
    //bridge.add_vid(pkt2,222);
    //bridge.add_vid(&pkt2[6],222);
    //bridge.add_vid(pkt3,333);
    //bridge.add_vid(&pkt3[6],444);
    //bridge.print();
    // Make some random packets
    /*
      for (int i=0;i<3;i++){
      MACADDR a,b;
      a.random_mac();
      b.random_mac();
      for (int j=0; j<6;j++) {
	pkt[j]=a.mac[j];
	pkt[j+6]=b.mac[j];
      }

      uint32_t inter=rand()%16;
      uint32_t vid=rand()%4096;
      bridge.map_pkt(inter,pkt);
      bridge.add_vid(b.mac,vid);
    }
    bridge.print();
    // add some random vlans
    for (int i=0;i<3;i++){
      MACADDR a;
      a.random_mac();
    uint32_t vid=rand()%4096;
      bridge.add_vid(a.mac,vid);
    }
    bridge.print();
    */

    unsigned int microsecond = 1000000;
    bridge.start_listener();
    for (;;) {
      bridge.print();
      usleep(10 * microsecond);//sleeps for 10 second
    }
}
