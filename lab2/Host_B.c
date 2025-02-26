#include "Sim_Engine.h"
#include "Functions.h"

int expected_seqnum;
struct pkt ackpkt;

/* Called from layer 5, passed the data to be sent to other side */
void B_output(struct msg message) {
  /* DON'T IMPLEMENT */
}

/* Called from layer 3, when a packet arrives for layer 4 */
void B_input(struct pkt packet) {

	//checking for coorrupt pkt, if not correct send duplicate ack
	if(packet.checksum != set_checksum(packet)) {
		ackpkt.acknum = !expected_seqnum;
		ackpkt.checksum = set_checksum(ackpkt);
		tolayer3(B,ackpkt);
		return;
	}
	//checking if packet is received in order
	if(packet.seqnum == expected_seqnum){
		tolayer5(B,packet.payload);
		ackpkt.acknum = expected_seqnum;
		ackpkt.checksum = set_checksum(ackpkt);
		tolayer3(B, ackpkt);
		expected_seqnum = !expected_seqnum; 
	}
	//handle possible dupes
	else {
		ackpkt.acknum = !expected_seqnum;
		ackpkt.checksum = set_checksum(ackpkt);
		tolayer3(B,ackpkt);
	}
}

/* Called when B's timer goes off */
void B_timerinterrupt() {
  /* TODO */
}  

/* The following routine will be called once (only) before any other */
/* Host B routines are called. You can use it to do any initialization */
void B_init() {
	expected_seqnum = 0;
}

