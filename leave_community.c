// checking the maximum score of each node, for belongingness to each of the communities; if a node has a very low score in a community, it will choose to leave the community; //communities with low scores for all nodes in it, will disappear
extern int buck;	
extern int KCORE;

int leave_community(CLUSTER **head, CLUSTER *network, CGRAPH *graph, int no_node, int phase){	

	int i, j, k, flag, status = 0;;
	NODE *temp2, *temp3, *temp4;
	CLUSTER *temp5, *temp6;
	
	//printf("\n\nNode Removals\n");
	temp5 = *head;
	while(temp5 != NULL){
	
		flag =  0;		
		//eliminating the commmunity as a whole if, the node trying to form the community has score less than the maximum score among all communities( within a range)
		/*while((temp5 -> vertex -> score < (temp5 -> avg_comscore/temp5 -> comm_count)*(1- (float)EPS/100)  && temp5 -> avg_comscore/temp5 -> comm_count <=1  ) || temp5 -> vertex -> score == 0 ){
			//printf("Community enforced by %d removed for it itself is weak. Score %f\t Averagesum %f count %d\n", temp5 -> vertex -> index, temp5 -> vertex -> score , temp5 -> avg_comscore, temp5 -> comm_count);
			delete_community(head, network, temp5 -> vertex -> index, 1);
			temp6 = temp5 -> next;
			temp5 -> next = NULL;
			temp5 = temp6;
			if(temp5 == NULL){
				flag = 1;
				break;	
			}
		}	
		if(flag == 1)
			break;*/
		//printf("%d:::\t", temp5 -> vertex -> index);
		if(temp5 -> change){
			temp3 = temp5 -> vertex;
			temp2 = temp3 -> next;
			i = 0;	
			while(temp2 != NULL){				
				//eliminating nodes with score less than the maximum score among all communities( within a range)
				//if(((network[temp2 -> index].comm_count >1 && temp2 -> score < (network[temp2-> index].avg_comscore/network[temp2-> index].comm_count) * (1- (float)EPS/100)) || temp2 -> score == 0 ) && temp2 -> phase == phase ){
				if(temp2 -> score == 0 || (network[temp2 -> index].comm_count >1 && temp2 -> score < (float)network[temp2 -> index].bucket/buck && temp2 -> phase == phase )){
					status = 1;
					i = 1;
					//printf("%d\t", temp2 -> index);				
					temp4 = temp5 -> vertex -> next;
					k = 0;
					while(temp4 != NULL && k < graph[temp2 -> index].degree ){			
						if(graph[temp2 -> index].link[k] == temp4 -> index){
							temp4 -> ncount -=1;
							temp4 = temp4 -> next;
							k++;
						}
						else if(graph[temp2 -> index].link[k] < temp4 -> index){
							if(graph[temp2 -> index].link[k] == temp5 -> vertex -> index )
								temp5 -> vertex -> ncount -=1;
							k++;							
						}
						else
							temp4 = temp4 -> next;
					}
					temp5 -> count -=1;							
					if(temp5 -> count <= KCORE ){
						delete_community(head, network, temp5 -> vertex -> index, 1);
						temp6 = temp5 -> next;					
						temp5 -> next = NULL;
						temp5 = temp6;					
						flag = 1;	
						break;
					}
					else{						
						if(temp2 -> score){						
							network[temp2-> index].comm_count--;
							
							j = (temp2 -> score - 0.01)*buck;
			   				network[temp2 -> index].bcount[j]--;
			   				
			   				j = (temp2 -> df - 0.01)*buck;
			   				network[temp2 -> index].dfbcount[j]--;
						}
		   				
						temp3 -> next = temp2 -> next;
						temp4 = temp2;
						temp2 = temp2 -> next;
						free(temp4);	
					}					
				}
				else{						
					temp3 = temp2; 
					temp2 = temp2 -> next;
				}
			}
			if(!flag && !i)
				temp5 -> change = 0;
		}
		if(!flag)				
			temp5 = temp5 -> next;
		//printf("\n");
	}	
	//printf("\nafter node leave \n");
	//display_network(head, graph);	
	return status;
}
