//Wed Jun 05 13:02:32 IST 2013 
//Garisha 
// adjcency list representation of input graph in tab separated format

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <malloc.h>
#include <time.h>


#define byte 32

/******************** CHANGE PARAMETER VALUES HERE ****************************/

//#define KCORE 2 //a node will have atleast KCORE connections (computation time is decreased with increase in value of KCORE beacuse of rapid filtering of nodes)
//#define OVL 0.6 //allow the amount(fraction) of overlap before the smaller of the communities is removed

/******************** CHANGE PARAMETER VALUES HERE ****************************/
int KCORE = 2;
float OVL = 0.6;
int buck;

typedef struct FullGraph
{
	int degree;		//number of adjacents
	int name;		//as in the input edge-list
	int *link;		//to next adjacent node
}CGRAPH;

typedef struct node
{
	int index; 		//node's mapping index
	int ncount;		//#adjacents within the cluster/community (where this node is)
	int mark;		//flag used for more than one pusposes 
	int phase;		//phase in which the node was added
	int dup;		//flag to mark that its own initiated community is a duplicate to this community
	float score;	//community connectedness score
	float df;		//neighborhood connectedness score
	struct node *next;	//link to next node in teh cluster/community
}NODE;

typedef struct cluster
{	
	int count;		// #nodes in a COMMUNITY
	int comm_count;	// #community of a NODE
	int maxphase;	// of a COMMUNITY (last phase where a node was added)
	int change;		// bit for denoting change in COMMUNITY
	int shift;		// bit for denoting change in a NODE's communities
	int bucket;		// stay cutoff score-bucket for NODE
	int dfbucket;	// join cutoff score-bucket for NODE
	int dfshift;	//flag to denote change in score buckets
	int *dfbcount;	// neighborhood connectedness score-buckets' counts for NODE
	int *bcount;	// community connectedness score-buckets' counts for NODE
	//float min_density;
	//int *comm;
	NODE *vertex;	//list of nodes of a COMMUNITY
	struct cluster *prev;	//COMMUNITY
	struct cluster *next;	//COMMUNITY
}CLUSTER;

CLUSTER* community_initialize(CLUSTER**, CGRAPH *, int);
void delete_community(CLUSTER **, CLUSTER *, int, int);
int leave_community(CLUSTER **, CLUSTER*, CGRAPH *, int, int);
void merge_list(CLUSTER **, CLUSTER *, int, int, CGRAPH *, int);
void community_detection(CLUSTER **, CLUSTER *, CGRAPH *, int );
//int hamming_distance(char *, char *, int );

void display_graph(CGRAPH *, int);
void display_network(CLUSTER **, CGRAPH *);
//void form_node_community(CLUSTER *, CLUSTER **, int);
//void display_node_community(CLUSTER *, int, CGRAPH *);
//void output_file(CLUSTER *, CLUSTER **, int , int , int);
//int check_adj(CGRAPH *, int, int);

#include "community_initialize.c"
#include "delete_community.c"
#include "leave_community.c"
#include "merge_list.c"
#include "cluster_detection.c"
//#include "inverse_hamming_distance.c"

int main(int argc, char *argv[]){

	time_t start_time, end_time;

	int no_node;
	int i, j, vtx, adj, *temp2, mid, ll, ul, directed;
	float avg_degree = 0, edge_density;
	char *syscmd, string[32];	
	
	FILE *fp, *p;

	CGRAPH *graph;
	CLUSTER **head, *network, *temp5, *temp6;
	NODE *temp3, *temp4;

	if(argc < 2){
		printf("less input arguments.enter file name\n");
		exit(1);
	}
	else if(argc == 4){
		sscanf(argv[2], "%d", &KCORE);
		sscanf(argv[3], "%f", &OVL);
	}
	syscmd = (char *)malloc(200*sizeof(char));
	
	fp = fopen(argv[1],"r");
	fscanf(fp, "%d\t%d\n", &vtx, &adj);
	sprintf(string, "^%d\t%d$", adj, vtx);
	fclose(fp);
	
	sprintf(syscmd, "grep -e '%s' %s| wc -l ", string, argv[1]);
	p = popen(syscmd, "r");
	fscanf(p,"%d", &directed);
	pclose(p);
	fprintf(stderr, "\ndirected %d", directed); 
	
	if(directed == 0){
		sprintf(syscmd,"awk 'BEGIN{OFS=\"\t\";} {print $2,$1}' %s > temp", argv[1]);
		system(syscmd);
		sprintf(syscmd,"cat %s temp|sort -k 1,1n -k 2,2n > temp1", argv[1]);
		system(syscmd);
	}
	else{
		sprintf(syscmd,"sort -k 1,1n -k 2,2n %s > temp1", argv[1]);	
		system(syscmd);	
	}
	
	system("cut -f 1 temp1 > temp");
	sprintf(syscmd,"sort -n temp| uniq -c > %s.uniq", argv[1]);
	system(syscmd);
	system("rm temp");
	
	sprintf(syscmd, "wc -l %s.uniq", argv[1]);	
	p = popen(syscmd, "r");
	fscanf(p,"%d", &no_node);
	pclose(p);
	fprintf(stderr, "\nNumber of nodes:\t%d\n", no_node);	

	graph = (CGRAPH *)malloc(no_node *sizeof(CGRAPH));
	temp2 = (int *)malloc(no_node * sizeof(int));
	
	sprintf(syscmd, "%s.uniq", argv[1]);
	fp = fopen(syscmd,"r");
		
	head = (CLUSTER **)malloc(sizeof(CLUSTER *));
	network = (CLUSTER *)malloc(no_node *sizeof(CLUSTER));
	*head = network;
	
	for (i=0;i<no_node;i++){
		fscanf(fp,"%d %d\n",&graph[i].degree, &graph[i].name);
		//printf("%d\t%d\t%d\n", i, graph[i].name, graph[i].degree);
		graph[i].link = (int *) malloc(graph[i].degree* sizeof (int));
		temp2[i] = 0;
		avg_degree += graph[i].degree;
	}
	fclose(fp);
	
	
	avg_degree /= no_node;	
	edge_density = avg_degree/(no_node -1);
	fprintf(stderr, "Average degree %f\nEdge Density %f\n", avg_degree, edge_density);
	if(avg_degree > 20)
		buck = avg_degree;
	else
		buck = 20;

	//graph as the adjacency list	
	fp = fopen("temp1","r");
	
	if(graph[0].name == 0 && graph[no_node -1 ].name == no_node -1){
		while((fscanf(fp,"%d\t%d\n", &vtx, &adj)) != EOF ){
			graph[vtx].link[temp2[vtx]] = adj;
			temp2[vtx]++;
		}
	}
	
	else if(graph[0].name == 1 && graph[no_node -1 ].name == no_node){
		while((fscanf(fp,"%d\t%d\n", &vtx, &adj)) != EOF ){
			graph[vtx-1].link[temp2[vtx-1]] = adj-1;
			temp2[vtx-1]++;
		}
	}
	
	else{	
		j = 0; i = 0;
		while((fscanf(fp,"%d\t%d\n", &vtx, &adj)) != EOF ){
			//printf("%d\t%d\n", vtx, adj);
			//printf(" %d\n" , k++); 
			if( graph[j].name != vtx  || (j == 0 && i == 0)){
				if(!(j == 0 && i == 0)){
					while(graph[j].name != vtx)
						j++;
				}
				//i = binary_search(graph, 0, no_node, adj );
				ll = 0;
				ul = no_node;
				while(ll <= ul){
					mid = (ll + ul)/2;
					if( adj == graph[mid].name){
						i =  mid;
						break;
					}
					else if( adj < graph[mid].name)
						ul = mid -1;
					else
						ll = mid +1;
				}	
			}
			else{
				//i = binary_search(graph, i + 1, no_node, adj );
				if( adj == graph[i + 1].name )
					i = i + 1;
				else{
					ll = i + 2;
					ul = no_node;
					while(ll <= ul){
						mid = (ll + ul)/2;
						if( adj == graph[mid].name){
							i =  mid;
							break;
						}
						else if( adj < graph[mid].name)
							ul = mid -1;
						else
							ll = mid +1;
					}
				}	
			}					
			graph[j].link[temp2[j]] = i;
			temp2[j]++;
		}
	}
	fclose(fp);
	free(temp2);
	system("rm temp1");

	/*fp = fopen("temp", "w");
	for( i = 0 ; i < no_node; i++){
		for ( j = 0; j < graph[i].degree; j++)
			fprintf(fp, " %d\t", graph[graph[i].link[j]].degree);
		fprintf(fp, "\n");
	}
	fclose(fp);
	
	sprintf(syscmd, "awk '{split($0, w); s=\"\"; sum = 0; for( i = 1; i <= asort(w); i++){ sum+= w[i]; s = s w[i] \"\\t\"}; sum = sum/NF; print NF \"\\t\" w[1] \"\\t\" sum \"\\t\" w[NF] \"\\t\" s| \"sort -n\"}' temp > %s_degree_dist", argv[1]);*/
	//system(syscmd);
	//printf("%s", syscmd);
	//system("rm temp");
	//free(syscmd);

	//display_graph(graph, no_node);		
	fprintf(stderr, "Graph constructed\n"); 
	
	/******************* COMMUNITY DETECTION ********************************/
	start_time = time(NULL);
	
	//initialize the communities
	network = community_intialize(head, graph, no_node);
	
	//call the cluster detection function	
	community_detection(head, network, graph, no_node);
	
	end_time = time(NULL);
	/************************************************************************/
	
	fprintf(stderr, "\ntime:%.f secs\n", difftime(end_time, start_time));
	
	display_network(head, graph);
	
	//destroy the graph and the network and free all pointer variables	
	for(i=0; i< no_node; i++){
		free(graph[i].link);
		network[i].bcount = NULL;
		network[i].dfbcount = NULL;
		//free(network[i].comm);
	}
	temp5 = *head;
	while(temp5 != NULL){
		temp4 = temp5 -> vertex;
		while (temp4 != NULL){
			   temp3 = temp4;
			   temp4 = temp4->next;
			   free(temp3);
		}	
		temp5 -> vertex = NULL;
		temp5 -> prev = NULL;	
		temp6 = temp5 -> next;
		temp5 -> next = NULL;
		temp5 = temp6;
	}
	free(graph);
	free(network);
	free(head);
	return;	
}

/*void output_file(CLUSTER *network, CLUSTER **head, int no_node, int iter, int phase){
	int i, count;
	CLUSTER *temp5;
	NODE *temp2;
	temp5 = *head;
	FILE *fp;
	char syscmd[100];
	
	count = 0;
	for(i = 0; i< no_node; i++){
		if(count < network[i].comm_count)
			count = network[i].comm_count;
	}
	sprintf(syscmd, "output_%d_%d", phase, iter);
	fp = fopen(syscmd, "w");
	fprintf(fp, "Id\t");
	for(i = 0; i< count-1; i++)
		fprintf(fp, "comm_%d\t", i+1);
	fprintf(fp, "comm_%d\n", i +1);
	for(i = 0; i< no_node; i++){
		if(network[i].comm_count != 0){
			count = 0;
			fprintf(fp,"%d.0\t", i);
			while(count < network[i].comm_count)
				fprintf(fp, "%d\t", network[i].comm[count++]);
			fprintf(fp, "\n");
		}
		else
			fprintf(fp,"%d.0\n", i);
	}
}*/	

void display_graph(CGRAPH *graph, int no_node){
	
	int i, j;	
	//printf("\nGraph as adjacency list:\nNode: (Degree)\tAdjacents\n");	
	for(i=0; i< no_node; i++){
		//temp0 = graph[i].link;
		j = 0;
		//printf("%d:\t( %d )\t", graph[i].name, graph[i].degree);
		printf("%d\t", graph[i].name);
		while(j < graph[i].degree -1){
			printf("%d\t", graph[graph[i].link[j]].name);
			j++;
		}
		printf("%d\n",graph[graph[i].link[j]].name);
	}
}

void display_network(CLUSTER **head, CGRAPH *graph ){
	NODE *temp0;
	CLUSTER *temp;
	//int cluster_count=0;
	//printf("\n\nclusters identified:\n\nIndex (Size) -> \t(Nodes: Score)*\n\n");	
	temp = *head;
	while(temp != NULL){
		temp0 = temp -> vertex;
		if(temp0 != NULL){
			//cluster_count ++;
			//printf("%d ( %d ) -> \t", cluster_count, temp -> count);
			while(temp0 -> next != NULL){
				printf("%d\t",graph[temp0->index].name);
				//printf("%d: %f\t",graph[temp0->index].name, temp0 -> score);
				temp0 = temp0 -> next;
			}
			printf("%d\n",graph[temp0->index].name);
			//printf("%d: %f\n",graph[temp0->index].name, temp0 -> score);
		}
		temp = temp -> next;		
	}
}

/*void form_node_community(CLUSTER *network, CLUSTER **head, int no_node){
	int i;
	CLUSTER *temp5;
	NODE *temp2;
	temp5 = *head;
	for(i = 0; i< no_node; i++)
		network[i].comm_count = 0;
	while(temp5 != NULL){
		temp2 = temp5 -> vertex;		
		while(temp2 != NULL){
			network[temp2 -> index].comm[network[temp2 -> index].comm_count] = temp5 -> vertex -> index;
			network[temp2 -> index].comm_count++;
			temp2 = temp2 -> next;
		}
		temp5 = temp5 -> next;
	}	
}*/

/*void display_node_community(CLUSTER *network, int no_node, CGRAPH *graph){
	int i, count;
	printf("\nCommunities for node; \n", i);
	for(i = 0; i< no_node; i++){
		if(network[i].comm_count != 0){
			count = 0;
			printf("%d:\t", graph[i].name);
			while(count < network[i].comm_count)
				printf("%d\t", graph[network[i].comm[count++]].name);
			printf("\n");
		}
		else
			printf("%d:\n", graph[i].name);
	}
}*/

/*int check_adj(CGRAPH *graph, int n1, int n2){
	int mid, ll, ul;
	ll = 0;
	ul = graph[n1].degree;
	while(ll <= ul){
		mid = (ll + ul)/2;
		if( n2 == graph[n1].link[mid])
			return 1;
		else if( n2 < graph[n1].link[mid])
			ul = mid - 1;
		else
			ll = mid + 1;			
	}
	return 0;
}*/
