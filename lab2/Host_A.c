#include "Sim_Engine.h"
#include "Host_A.h"
#include "Functions.h"

bool timeron = false;
int A_nextseqnum, A_expected_seqnum;
struct pkt outpkt, A_lastpkt;

/* Called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message) {
	outpkt.seqnum = A_nextseqnum; //set the sequence nr 1/0
	outpkt.acknum = 0;
	
	strncpy(outpkt.payload, message.data, PAYLOADSIZE); //copy the data in msg to our packet payload
	outpkt.checksum = set_checksum(outpkt); 			//set the checksum

	A_lastpkt = outpkt;									//copy the packet in case we need resend it
	tolayer3(A,outpkt);

	if(!timeron){
		starttimer(A,TIMEOUT);
		timeron = true;
	}
}

/* Called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet) {
	//if pkt ok, fix sequence nr and send next
	if(packet.acknum == A_expected_seqnum && packet.checksum == set_checksum(packet)){
		if(timeron){
			stoptimer(A);
			timeron = false;
		}
		A_nextseqnum = !A_nextseqnum;
		A_expected_seqnum = !A_expected_seqnum;
		return;
	}
	else{
		//printf("WAITING FOR TIMER");
	}
}

/* Called when A's timer goes off */
void A_timerinterrupt() {
	tolayer3(A, A_lastpkt);
	starttimer(A, TIMEOUT);
	timeron = true;
} 

/* The following routine will be called once (only) before any other */
/* Host A routines are called. You can use it to do any initialization */
void A_init() {
	A_nextseqnum = 0;
	A_expected_seqnum = 0;
}

void send_enqueue(){

}
void send_dequeue(){

}
