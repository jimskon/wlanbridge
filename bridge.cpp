#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <bits/stdc++.h>

#include <sys/select.h>

#include <fstream>
#include "brmap.h"
#include "iface.h"
#include "pdu/pdu.h"
#include "icmp4.h"
#include "InputParser.h"
#include "logger.h"

#define BUFLEN 9000
#define VLAN_SIZE 4  /* Number of bytes for vlan tagging */
/* Interface "0" will never be used, since the bridge considered this a flood. */
#define MAX_I 10 // Maximun number of supported interfaces.

void printpacket(const char* msg, const unsigned char* p, size_t len) {
    size_t i;
    std::cout << msg << " len=" << len << std::endl;
    for(i=0; i<len; ++i) {
      std::cout << std::hex << (int) p[i] << ":";
    }
    std::cout << std::dec << endl;
}

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

/* Call with interface name to set correct interface state */
int setInterfaceState(string interface, logger &log) {
        string command="ethtool -K ";
        command += interface + " ";
	command += "tx off rx off gso off tso off gro off lro off";
	log(0) << command << std::endl;
	// Run command;
        const char *com = command.c_str();
	const int ret = std::system(com);
	return ret;
}


int
main( int argc, char *argv[] )
{
        logger  log;
	iface  itf[MAX_I];
	struct timeval tv;
	fd_set rfds;
	int    fd[MAX_I];
	int    maxfd;
	int    n;
	int    len;
	int    debug;
	int    interfaces;
	int    global_vid = -1; 
	brmap  a_brmap( log );
        ofstream log_out;
	
	uint8_t *x;

	if( argc < 3 ) {
		std::cerr << "usage: " << argv[0] << " if1 if2 ...-f log_file" << std::endl
		          << "First interface is the tagged wired interface" << std::endl
		          << "Other interfaces are the wlan interfaces" << std::endl
		          << "DEBUG=1 or 2 for debug output"    << std::endl
		;
		exit(1);
	}

	debug = 0;
	if(getenv("DEBUG")) {
		debug = atoi(getenv("DEBUG"));
		std::cout << "DEBUG: " << debug << std::endl;
	}

	log.level( debug );

	// Start count for interfaces
	interfaces = argc-1;

	InputParser input(argc, argv);

	// Get log file
	const std::string &filename = input.getCmdOption("-f");
	if (!filename.empty()){
	  std::cout << "Log file: " << filename << std::endl;
	  interfaces = interfaces-2;
	  log.path(filename);  
	  // send stdout and stderr to the log file
	  auto ret = freopen(filename.c_str(),"a",stderr);
	  //std::cout << "Logfile:" << ret << std::endl;
	}	

	// Get vid
	const std::string &vid_str = input.getCmdOption("-v");
	if (!vid_str.empty()){
	  if (!is_number(vid_str) || stoi(vid_str) < 1 || stoi(vid_str) > 4095) {
	    std::cerr << "VID must be an integer between 1-4095" << endl;
	    exit(1);
	  }
	  global_vid = stoi(vid_str);
	  std::cout << "Global VID: " << global_vid << std::endl;
	  interfaces = interfaces-2;
	}	

	if (interfaces >=  MAX_I) {
	  std::cerr << "Bridge error: too many interfaces specificied.  Max is " << MAX_I-1 << std::endl;
	  exit(1);
	}
	for (int i = 1; i<=interfaces; i++) {
	  std::cout << argv[i] << std::endl;
	  setInterfaceState(argv[i],log);
	  assert( itf[i].bind( argv[i] ) == 0 );
	  assert( itf[i].promisc( true ) == 0 );
	  log(1) << itf[i] << std::endl;
	}
	
	std::cout << "Start listener" << std::endl;

	/*  Start bridge listener (for vlan assignments) */
        a_brmap.start_listener();
	
	x = (uint8_t *) malloc( BUFLEN );
	assert( x != NULL );

	for (int i = 1; i <= interfaces; i++) {
	  fd[i] = itf[i].socket();
	}
	
	maxfd = 0;
	for (int j = 1; j <= interfaces; j++) {
	  if (maxfd < fd[j])
	    maxfd = fd[j];
	}

	for( ;; ) {
	        pdu pkt( BUFLEN ); 
	
		FD_ZERO( &rfds );

		for (int i = 1; i<=interfaces; i++) {		
		  FD_SET( fd[i], &rfds );
		}
		
		tv.tv_sec  = 1;
		tv.tv_usec = 0;

		n = select( maxfd+1, &rfds, NULL, NULL, &tv );
		if( n < 0 ) {
			perror( "select()" );
			break;
		}

		if( n == 0 ) {
			/*
			 * timeout on select() -- for now, just signal that we're still alive.
			 * this would be a nice place to delete expired BNG entries though
			 * (no need for timers/threads/locks since it'a all sequential).
			 */
			log( 3 ) << "waiting..." << std::endl;
			continue;
		}

		/* This interface will add vlan tags on egress, and remove them on ingress.*/
		if( FD_ISSET( fd[1], &rfds )) {
		        n = itf[1].recv( pkt );

	                if( n < 0 ) {
				perror( itf[1].name() );
				break;
			}

			if( n == 0 )
				continue;


			/* Handle vlan tags on ingress */
			int s_vid = pkt.vlan_untag();
			if (s_vid >= 0) {			 
			  std::cout << "In VID: " << s_vid << std::endl;
			}
			(void)pkt.filter( log( 1 ));
			
			/*
			off = pkt.payload_off();
			pdu_eth pkt_eth( pkt, off );
			log(1) << " > " << pkt_eth << std::endl;

			off = pkt_eth.payload_off();
			pdu_ipv4 pkt_ipv4( pkt_eth, off );
			log(1) << "  > " << pkt_ipv4 << std::endl;
*/

			int vid;
			short d = a_brmap.map_pkt(1,pkt._x,&vid);
			if ( d > 0 && n > itf[d].mtu() ) {
			  log(1) << "Packet too long: " << n << " MTU " << itf[d].mtu() << std::endl;
				//log(1) << pkt;
				log(1) << "Sending ICMP fragmentation needed" << std::endl;
				n = icmp4_gen_needfrag( x+128, BUFLEN-128, x, itf[d].mtu() );
				//(void)if1.send( x+128, n, sll );
				continue;
			}

			// Send to interface from map if not 1 or broadcast
			if( d > 1) {

			  if( (len = itf[d].send( pkt )) < 0 ) {
			               perror( itf[d].name() );
			               break;
			  }
			    log << "1>2 " << len << "  " << std::endl;
			    
			} else if (d == 0) { // Broadcast
			  //std::cout << "Broadcast" << std::endl;
			  // Don't send back to first interface
			  for (int j = interfaces; j > 1; j--) {
			    if( (len = itf[j].send( pkt )) < 0 ) {
			      perror( itf[j].name() );
			    }
			  }
			}


		}
		/* These interfaces (>1) will add vlan tags on ingress, and remove on egress
		   if bound for interface 1 (the wired interface) */
		for (int i = 2; i<=interfaces; i++) {
		  if( FD_ISSET( fd[i], &rfds )) {

		        n = itf[i].recv( pkt );

			if( n < 0 ) {
				perror( itf[i].name() );
				break;
			}

			if( n == 0 )
				continue;

			(void)pkt.filter( log( 1 ));

			int vid;
			short d = a_brmap.map_pkt(i,pkt._x,&vid);
			// Use global vid if requested
			if (global_vid > 0) {
			  vid = global_vid;
			}
			//short d = 0;

			if ( d > 0 && n > itf[d].mtu() ) {
			        log(1) << "Packet too long: " << n << " MTU " << itf[d].mtu() << std::endl     ;
				log(1) << "Sending ICMP fragmentation needed" << std::endl;
				n = icmp4_gen_needfrag( x+128, BUFLEN-128, x, itf[i].mtu() );
				//(void)itf[i].send( x+128, n, sll );
				continue;
			}

			//log(1) << pkt << std::endl;
			/* Only tag for interface 1 */
			if (d == 1) {
			  if (vid>=0) {
			    pkt.vlan_tag((uint32_t) vid);
			  }
			  if( (len = itf[1].send( pkt )) < 0 ) {
			    perror( itf[1].name() );
			    break;
			  }
			  
			} else if (d != 0 and d != i) { //send to another client
			// Send to interface d if dest is d
			  if( (len = itf[d].send( pkt )) < 0 ) {
			               perror( itf[d].name() );
			               break;
			  }
			  log << i << ">" << d << ":" << len << "  " << std::endl;
			} else if (d == 0) { // Broadcast
			  //std::cout << "Broadcast" << std::endl;
			  for (int j = interfaces; j > 0; j--) {
			    
			    if (j == 1) {
			      if (vid>=0) {
				pkt.vlan_tag((uint32_t) vid);
			      }

			      if( (len = itf[j].send( pkt )) < 0 ) {
				perror( itf[j].name() );
			      }
			      log << i << ">" << d << ":" << len << "  " << std::endl;

			    } else {
			      if (j != i) {
				if( (len = itf[j].send( pkt )) < 0 ) {
			               perror( itf[j].name() );
			               break;
				}
			      }
			    }
			  }
			} else {
			  std::cout << "Bad interface: " << d << std::endl;
			}
			if ( debug > 1 )
			  a_brmap.print();
		  }
		}
	}

	free( x );

	return 0;
}

/*EoF*/
