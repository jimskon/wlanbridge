/* 
   Program to send test vid/mac messages to the RGNets bridge.
   The MAC address is randomly generated.
   Jim Skon, RGNets, 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <iostream>
#include <sstream>
#include "brmap.h"

using namespace std;


void error(const char *msg);


void str_to_mac(string mac_str, unsigned char * mac) {
  unsigned int value;
  char ignore;
  istringstream iss(mac_str ,istringstream::in);

  iss >> hex;

  for(int i=0;i<5;i++) {
    iss >> value >> ignore;
    mac[i]=value;
  }
  iss >> value >> dec;
  mac[5]=value;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    for (;;) {
      cout << "Please enter a vid: ";
      uint32_t vid;
      cin >> vid;
     
      MACADDR mac;
      cout << "Enter MAC address in form xx:xx:xx:xx:xx:xx (return for random): ";
      string mac_str;
      unsigned char octets[6];
      cin.ignore();
      getline(cin,mac_str);
      if (mac_str.size() > 0) {
	str_to_mac(mac_str,octets);
	mac=MACADDR(octets);
      } else {
	mac.random_mac();
      }
      cout << "Sending: " << mac << " VID: " << vid << endl;
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) 
        error("ERROR opening socket");
      if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
      struct vid_mess aVid_mess;
      bzero((void *)&aVid_mess,sizeof(struct vid_mess));
      aVid_mess.be_vid = vid;
      memcpy(aVid_mess.be_haddr,mac.mac,6);
      
      n = write(sockfd,&aVid_mess,sizeof(struct vid_mess));
      if (n < 0) 
	error("ERROR writing to socket");
      close(sockfd);
    }
    
    return 0;
}
