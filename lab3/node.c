#include "node.h"

void sendpkt (struct distance_table *table, int node);

void rtinit(struct distance_table *table, int node) {
	// Tables are already initialized
	sendpkt(table, node);
	printdt(table, node);
}

void rtupdate(struct distance_table *table, int node, struct rtpkt *pkt) {
	bool updated = false;
	int source = node;
	struct distance_table *updates = malloc(sizeof(struct distance_table));

	//initialize the distance table "updates" to make sure it's not filled with garbage
	memcpy(updates, table, sizeof(struct distance_table));

	//rn only updates if new cost is better
	for (int i = 0; i < 4; i++){
		if(is_neighbor(source, i)){
			for (int j = 0; j < 4; j++){
				int new_cost = table->costs[source][i] + pkt->mincost[j];
				if(updates->costs[source][j] > new_cost){
					updates->costs[source][j] = new_cost;
				}
			}
		}
	}
	for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				if(updates->costs[i][j] != table->costs[i][j]){
					updated = true;					
					table->costs[i][j] = updates->costs[i][j];
				}
			}
		}
	if(updated){
		sendpkt(updates, node);
		updated = false;
	}
	free(updates);
	printdt(table, node);
}

void sendpkt(struct distance_table *table,int node){
	int source = node;
	struct rtpkt distpkt;			//create pkt
	distpkt.sourceid = source;							//set sourceID
	//construct mincost & destinationID for pkt to be sent to neighbours
	for(int i = 0; i < 4; i++){
		if(is_neighbor(source, i)){
			printf("Setting mincost[%d] = table->costs[%d][%d] = %d\n", i, source, i, table->costs[source][i]);
			distpkt.mincost[i] = table->costs[source][i];	
			
		}
		else if(!is_neighbor(source, i)){
			if (source == i) {distpkt.mincost[i] = 0;}
			else {distpkt.mincost[i] = INF;}
		}
	}
	for (int i = 0; i < 4; i++){
		if (is_neighbor(source, i)){
			distpkt.destid = i;	
			tolayer2(distpkt);
		}
	}
}