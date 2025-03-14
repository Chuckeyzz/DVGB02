#include "node.h"

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	sendpkt(table, node);
	printdt(table, node);
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	bool changed = false;
	int source = node;
	int via_node = pkt->sourceid;
	struct distance_table *updates = malloc(sizeof(struct distance_table));

	//initialize the distance table "updates" to make sure it's not filled with garbage
	memcpy(updates, table, sizeof(struct distance_table));
	//purpose of updates table is to prevent corruptions of data and garbage values in *table  
	
	//update the cost if a better one exists
	for (int dest = 0; dest < 4; dest++){
		if(updates->costs[via_node][dest] > pkt->mincost[dest]){
				updates->costs[via_node][dest] = pkt->mincost[dest];
			}
		}
	//actually update distance table
	for (int via = 0; via < 4; via++){
			for (int dest = 0; dest < 4; dest++){
				if(updates->costs[via][dest] != table->costs[via][dest]){
					changed = true;					
					table->costs[via][dest] = updates->costs[via][dest];
				}
			}
		}
	//if the distance table has been changed, sent it to neighbours
	if(changed){
		sendpkt(table, node);
		changed = false;
	}
	free(updates);
	printdt(table, node);
}

void sendpkt(struct distance_table *table,int node){
	int source = node;
	struct rtpkt distpkt;								//create pkt
	distpkt.sourceid = source;							//set sourceID
	
	//construct mincost & destID for pkt to be sent to neighbours (changed because of lab specification checks, thanks chatGPT)
	for (int d = 0; d < 4; d++){
		int best = INF;
		for (int via = 0; via < 4; via++){
			int cost;
			if(via == node){
				cost = table->costs[node][d];
			}
			else {
				cost = table->costs[node][via] + table->costs[via][d];
			}
			if(cost < best){
				best = cost;
			}
		}
		distpkt.mincost[d] = best;
	}
	//send the packets "UP UP AND AWAY"
	for (int i = 0; i < 4; i++){ 
		if (is_neighbor(source, i)){
			distpkt.destid = i;	
			tolayer2(distpkt);
		}
	}
}