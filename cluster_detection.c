extern int buck;
extern int KCORE;
extern float OVL;
void community_detection(CLUSTER **head, CLUSTER *network, CGRAPH *graph, int no_node){	

	int iteration, phase = 0, flag, dupli, status, i, j, k, max, min, mbuck; 
	
	const float lg2 = log10(2);
	NODE *temp2, *temp3, *temp4, *temp0;
	CLUSTER *temp5, *temp6;	
		
	while(1){
		iteration = 0;
		phase++;
		fprintf(stderr, "\nphase %d\t", phase);
		//ci = cluster_index;
		while(1){	
		
			//printf("before duplicate removal\n");
			//display_network(head, graph); 
			
			//eliminating duplicate communities	
			temp5 = *head;
			while(temp5 != NULL){
					
				flag = 0;	
				if(temp5 -> change){
					i = 0;						
					temp2 = temp5 -> vertex -> next;
					while(temp2 != NULL){
						//printf("%d\t", temp2 -> index);
						if(temp2 -> phase == phase)
							i = 1;
						if(temp2 -> mark == 0 && network[temp2 -> index].vertex != NULL){
							dupli = 1;
							temp3 = network[temp2 -> index].vertex -> next;
							temp4 = temp5 -> vertex -> next;
							while(temp3 != NULL && temp4 != NULL){
								if(temp3 -> index < temp4 -> index){
									if(temp3 -> index == temp5 -> vertex -> index){
										dupli++;
										temp3 -> mark = 1;
										temp0 = temp3;
									}
									temp3 = temp3 -> next;
								}
								else if(temp3 -> index > temp4 -> index)
									temp4 = temp4 -> next;
								else{
									dupli++;
									temp3 = temp3 -> next;
									temp4 = temp4 -> next;
								}
							}
							if(dupli >= OVL*temp5 -> count && dupli >= OVL*network[temp2 -> index].count ){
								if( temp5 -> count <= network[temp2 -> index].count ){
									//printf("Communtiy %d removed for duplication. Duplicate with %d\n", temp5 -> vertex -> index, temp2 -> index);
									if(dupli < temp5 -> count ) 
										temp0 -> dup = 1;
									if(!iteration)
										delete_community(head, network, temp5 -> vertex -> index, 0);
									else
										delete_community(head, network, temp5 -> vertex -> index, 1);
									temp6 = temp5 -> next;
									temp5 -> next = NULL;
									temp5 = temp6;
									flag = 1;
									break;
								}	
								else {
									//printf("Communtiy %d removed for duplication.Duplicate with %d\n.", temp2 -> index,temp5 -> vertex -> index );	
									if(dupli < network[temp2 -> index].count) 
										temp2 -> dup = 1;
									if(!iteration)
										delete_community(head, network, temp2 -> index, 0);
									else
										delete_community(head, network, temp2 -> index, 1);
									network[temp2 -> index].next = NULL;
								}
							}
						}
						temp2 = temp2 -> next ;
					}
					if( !flag && !i)
						temp5 -> maxphase = phase -1;
				}
				//printf("\n");
				if(!flag )
					temp5 = temp5 -> next;
			}
			//printf("After duplicate removal\n");
			//display_network(head, graph); 	
					
			//recomputation of score	
			//printf("communities that changed \n");
			temp5 = *head;		
			while(temp5 != NULL){
						
				if(temp5 -> change){	
					//printf("%d\t", graph[temp5 -> vertex -> index].name );
					temp2 = temp5 -> vertex -> next;
					
					while(temp2 != NULL){
						temp2 -> mark = 0;
						network[temp2 -> index].shift = 1;
						if(!iteration)
							network[temp2 -> index].dfshift = 1;
																	
						if(!iteration && temp2 -> phase == phase){
												
							if(temp2 -> ncount < KCORE){
								temp2 -> score = 0;
								temp2 -> df = 0;
							}
							else{
								network[temp2 -> index].comm_count +=1;
								
								temp2 -> score = (float)(temp2 -> ncount-KCORE + 1)/(temp5 -> count -KCORE);					
								k =( temp2 -> score - 0.01) * buck;
								network[temp2 -> index].bcount[k]++;
								
								temp2 -> df = (float)temp2 -> ncount/graph[temp2 -> index].degree;
								k =( temp2 -> df - 0.01) * buck;
								network[temp2 -> index].dfbcount[k]++;
							}			
						}
						else{											
							k =( temp2 -> score - 0.01) * buck;
							network[temp2 -> index].bcount[k]--;
								
							k =( temp2 -> df - 0.01) * buck;
							network[temp2 -> index].dfbcount[k]--;
							
							if(temp2 -> ncount < KCORE){
								temp2 -> score = 0;
								temp2 -> df = 0;
								network[temp2 -> index].comm_count--;
							}
							
							else{
								temp2 -> score = (float)(temp2 -> ncount-KCORE + 1)/(temp5 -> count -KCORE);					
								k =( temp2 -> score - 0.01) * buck;
								network[temp2 -> index].bcount[k]++;
								
								temp2 -> df = (float)temp2 -> ncount/graph[temp2 -> index].degree;
								k =( temp2 -> df - 0.01) * buck;
								network[temp2 -> index].dfbcount[k]++;	
							}
						}
						temp2 = temp2 -> next;
					}
					
					if(temp5 -> vertex -> ncount < KCORE){
						temp5 -> vertex -> score = 0;
						temp5 -> vertex -> df = 0;
					}
					else{
						temp5 -> vertex -> score = (float)(temp5 -> vertex -> ncount - KCORE + 1)/(temp5 -> count - KCORE);
						temp5 -> vertex -> df = (float)temp5 -> vertex -> ncount/graph[temp5 -> vertex -> index].degree;
					}
					//printf("\n");
				}
				//printf("\n------------------------------------------\n");				
				temp5 = temp5 -> next;	
			}		
					
			//printf("\nBuckets\n");// To find out the score separating the good scored communities from the rest for each node
			dupli = 0;		
			for( i = 0; i< no_node; i++){
				dupli += network[i].comm_count;
				if(network[i].vertex != NULL)
					dupli++;
				if(network[i].shift  && network[i].comm_count > 1){
					k = log10(graph[i].degree)/lg2 + 1;
					
					for(j = buck-1 ; j > 0; j--){
						if(network[i].bcount[j]){
							mbuck = j;
							break;
						}
					}	
					/*max = min = network[i].bcount[mbuck];
					for(j = mbuck-1 ; j > 0; j--){
						if(max - min > k){
							if(network[i].bcount[j] < network[i].bcount[j + 1] && network[i].bcount[j] <= network[i].bcount[j -1]){
								network[i].bucket = j + 1;
								break;
							}
						}
						else if(mbuck - j > buck/4 ){
							network[i].bucket = j;
							break;
						}
						else if(network[i].bcount[j] > max)
							max = network[i].bcount[j];
						else if(network[i].bcount[j] < min)
							min = network[i].bcount[j];
					}*/
					
					min = network[i].bcount[mbuck];
					for(j = mbuck-1 ; j > 0; j--){
						if(network[i].bcount[j] < min)
							min = network[i].bcount[j];
						if(network[i].bcount[j] == min && network[i].bcount[j] <= network[i].bcount[j -1]){
							network[i].bucket = j + 1;
							break;
						}
					}
					if( j == 0)
						network[i].bucket = 0;	
				}
				
				if(network[i].shift){
					/*printf("%d: %d :mbuck %d :k %d\n", graph[i].name, network[i].comm_count, mbuck, k);
					for( j = 0; j < buck ; j++)
						printf("%d\t", network[i].bcount[j]);
					printf("bucket %d\n", network[i].bucket);*/
					
					network[i].shift = 0;				
				}
			}
			fprintf(stderr, "%f\t", (float)dupli/no_node);//exit(0);
			
			//printf("\n\nIterative Node Removal and duplicate Community Removal\n");				
			if(!leave_community(head, network, graph, no_node, phase))
				break;
			iteration++;
			//printf("\n\nIteration %d:\t", iteration);		
		}
		//printf("After leave phase\n");
		//display_network(head, graph); 
		//display_node_community(network, no_node, graph);
		
		if(*head == NULL ){
			fprintf(stderr, "No communities exist with the given set of parameters. Try increasing OVL parameter value or lowering KCORE value( lowest being 2 )\n\n");
			exit(0);
		}
		
		//Bucketing for selective expansion		
		//printf("\nBuckets\n");// To find out the score separating the good scored communities from the rest for each node
		for( i = 0; i< no_node; i++){
			if(network[i].comm_count && network[i].dfshift){
				//k = log10(network[i].comm_count)/lg2;
				k = log10(graph[i].degree)/lg2 + 1;
				
				for(j = buck-1 ; j > 0; j--){
					if(network[i].dfbcount[j]){
						mbuck = j;
						break;
					}
				}	
				min = network[i].dfbcount[mbuck];
				for(j = mbuck-1 ; j > 0; j--){	
					if(network[i].dfbcount[j] < min)
						min = network[i].dfbcount[j];				
					if(network[i].dfbcount[j] == min && network[i].dfbcount[j] <= network[i].dfbcount[j -1]){
						network[i].dfbucket = j + 1;
						break;
					}				
				}
				if( j == 0)
					network[i].dfbucket = 0;	
			}
			
			if(network[i].dfshift){
				/*if(network[i].comm_count){
					printf("%d: %d :mbuck %d :k %d\n", graph[i].name, network[i].comm_count, mbuck, k);
					for( j = 0; j < buck ; j++)
						printf("%d\t", network[i].dfbcount[j]);
					printf("bucket %d\n", network[i].dfbucket);
				}*/
				network[i].dfshift = 0;			
			}
		}
		
		fprintf(stderr, "merging..\t");
		status = 1;		
		//Growing of communities	
		//printf("\n\nExpansion of:\n");
		temp5 = *head;
		while(temp5 != NULL){
			if(temp5 -> maxphase == phase ){
				//printf("\n\nCommunity %d:", graph[temp5 -> vertex -> index].name);
				temp2 = temp5 -> vertex -> next;						
				while(temp2 != NULL){					
					//simple expansion
					//if(temp2 -> phase  == phase && network[temp2 -> index].vertex != NULL && graph[temp2 -> index].degree  > temp2 -> ncount){
					//if(temp2 -> phase  == phase && graph[temp2 -> index].degree  > temp2 -> ncount){
					if(temp2 -> phase  == phase && (network[temp2 -> index].vertex != NULL ||  temp2 -> dup) && graph[temp2 -> index].degree  > temp2 -> ncount){
						//fprintf(stderr, ".");
						status = 0;
						temp5 -> maxphase = phase + 1;
						//printf("\ngrowing through %d: inserted\t",  graph[temp2 -> index].name);
						merge_list(head, network, temp5 -> vertex -> index, temp2 -> index, graph, phase);
					}		
					temp2 = temp2 -> next;
				}				
			}
			if(temp5 -> maxphase == phase + 1)
				temp5 -> change = 1;
			else
				temp5 -> change = 0;
			temp5 = temp5 -> next;
		}
		//printf("\n");
		if(status){
			//possible merging ??
			return;
		}
		
	}
}
