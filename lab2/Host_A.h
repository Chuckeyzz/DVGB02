#ifndef HOST_A_H
#define HOST_A_H

void A_output(struct msg message);
void A_input(struct pkt packet);
void A_timerinterrupt();
void A_init();
int set_checksum(struct pkt pkt);

#endif
