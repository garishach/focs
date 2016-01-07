extern int buck;
extern int KCORE;

CLUSTER* community_intialize(CLUSTER** head, CGRAPH *graph, int no_node){

	int i, j, k, max_degree=INT_MIN, last = -1;
	char **bin_matrix;
	
	NODE *temp2, *temp3, *temp4;
	CLUSTER *temp5, *network;

	network = *head;
	
	//building the community network-initially each node along with all its neighbours is a community; thus, network[i] is commmunity around node i
	// time complexity of no_node* degree_max^2
	for(i=0; i< no_node; i++){
		//printf("Node forcing the community: %d\n", i);
		//initializations		
		//network[i].min_density = 0;
		//network[i].comm = (int*)malloc(graph[i].degree * sizeof(int));
		network[i].comm_count = 0;			
		network[i].shift = 0;
		network[i].dfshift = 1;
		
		network[i].bcount = (int*)malloc(buck * sizeof(int));
		network[i].dfbcount = (int*)malloc(buck * sizeof(int));
		for( j = 0; j < buck; j++){
			network[i].bcount[j] = 0;
			network[i].dfbcount[j] = 0;
		}
			
		network[i].count = graph[i].degree +1;	
		
		if(graph[i].degree >= KCORE){
			//printf("%d\n", i);
			network[i].change = 1;	
			network[i].maxphase = 1;
			//printf("\tC: %d", network[i].count);

			temp3 = (NODE *)malloc(sizeof(NODE));
			temp3 -> next = NULL;
			temp3 -> index = i;
			temp3 -> mark = 0;
			temp3 -> phase = 0;
			temp3 -> dup = 0;
			network[i].vertex = temp3;
			
			if(last == -1){
				network[i].prev = NULL;
				*head = &network[i];
			}
			else{
				network[i].prev = &network[last];
				network[last].next = &network[i];
			}
			last = i;
			//printf("Nodes in the communtiy:\n");
			//printf("%d\t", i);
			temp2 = temp3;
			k = 0;
			while(k < graph[i].degree){			
				temp3 = (NODE *)malloc(sizeof(NODE));
				temp3 -> next = NULL;
				temp3 -> index = graph[i].link[k];				
				k++;
				//printf("%d\t", temp3 -> index);
				temp3 -> mark = 0;
				//computation of ncount
				temp3 -> ncount = 0;
				
				j=0;
				temp4 = network[i].vertex -> next;
				while( j < graph[temp3 -> index].degree && temp4 != NULL){
					if( graph[temp3 -> index].link[j] == temp4 -> index){
						temp4 -> ncount++;
						temp3 -> ncount++;
						j++;
						temp4 = temp4 -> next;
					}
					else if(graph[temp3 -> index].link[j] < temp4 -> index)
						j++;
					else
						temp4 = temp4 -> next;
				}
				
				temp3 -> phase = 1;	
				temp3 -> dup = 0;
				temp2 -> next = temp3;
				temp2 = temp2-> next;
			}
			if(max_degree < graph[i].degree)
				max_degree = graph[i].degree;
			
			network[i].vertex -> ncount = graph[i].degree;
		}		
		else{
			network[i].count = 0;
			network[i].prev = network[i].next = NULL;
			network[i].vertex = NULL;
		}		
	}
	network[last].next = NULL;
	
	return network;
}
