#include "Sim_Engine.h"
#include "Host_A.h"
#include "Host_B.h"
#include "Functions.h"

int set_checksum(struct pkt pkt) {
	int sum = 0; 
	sum += pkt.seqnum;
	sum += pkt.acknum;

	for (int i = 0; i < PAYLOADSIZE; i++){
		sum += (int)pkt.payload[i]; //get the sum for every payload character
	}
	return ~sum; //one's complement 
}