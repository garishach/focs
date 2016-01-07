//merge 2 linked lists(sorted and containing distinct elements in a list) into 1 of them( in this case 1st)
extern int KCORE;
void merge_list(CLUSTER **head, CLUSTER *network, int list1, int list2, CGRAPH *graph, int phase){
	
	int q, i, j, ins, ncount;
	float score;
	NODE *temp1, *temp2, *temp3, *p;
	
	q = 0;
	temp1 = network[list1].vertex;
	p = temp1 -> next;
	while(q < graph[list2].degree && p != NULL){
		if ( graph[list2].link[q] < p -> index ){	
			if( graph[list2].link[q] != network[list1].vertex -> index){				
				ins = graph[list2].link[q]; 
				ncount = 0;
				i = 0;
				temp2 = network[list1].vertex -> next;
				while( i < graph[ins].degree && temp2 != NULL){
					j = graph[ins].link[i];
					if(j < temp2 -> index){
						if(j == network[list1].vertex -> index)
							ncount++;
						i++;
					}
					else{
						if(j == temp2 -> index){
							if(temp2 -> phase != phase + 1)
								ncount++;
							i++;
						}
						temp2 = temp2 -> next;
					}
				}
				if(ncount < KCORE)
					score = 0;
				else
					//score = (float)(ncount-KCORE + 1 )/(network[list1].count -KCORE + 1);	
					score = (float)ncount/graph[ins].degree;
					
				if(score != 0 && (network[ins].comm_count < 1 || score > (float)network[ins].dfbucket/buck)){
					//printf("%d\t", graph[ins].name);
					temp3 = (NODE *)malloc(sizeof(NODE));
					temp3 -> index = ins;
					temp3 -> phase = phase +1;
					temp3 -> next = p;
				 	temp3 -> ncount = ncount;
				 	
				 	i = 0;
					temp2 = network[list1].vertex -> next;
					while( i < graph[ins].degree && temp2 != NULL){
						j = graph[ins].link[i];
						if(j < temp2 -> index){
							if(j == network[list1].vertex -> index)
								network[list1].vertex ->ncount++;						
							i++;
						}
						else{
							if(j == temp2 -> index){
								temp2 -> ncount++;
								if( temp2 -> phase == phase + 1)
									temp3 -> ncount++;
								i++;
							}
		 					temp2 = temp2 -> next;
						}
					}
					network[list1].count++;
									 	
				 	temp1 -> next = temp3;
					temp1 = temp3;					
				}
			}
			q++;
		}		
		else{			
			if( graph[list2].link[q] == p -> index ){
				q++ ;
			}
			temp1 = p;
			p = p -> next;	
		}
	}
	while(q < graph[list2].degree ){
		if( graph[list2].link[q] != network[list1].vertex -> index){
			ins = graph[list2].link[q]; 
			ncount = 0;
			i = 0;
			temp2 = network[list1].vertex -> next;
			while( i < graph[ins].degree && temp2 != NULL){
				j = graph[ins].link[i];
				if(j < temp2 -> index){
					if(j == network[list1].vertex -> index)
						ncount++;
					i++;
				}
				else{
					if(j == temp2 -> index){
						if(temp2 -> phase != phase + 1)
							ncount++;
						i++;
					}
					temp2 = temp2 -> next;
				}
			}
			if(ncount < KCORE)
				score = 0;
			else
				//score = (float)(ncount-KCORE + 1 )/(network[list1].count -KCORE + 1);	
				score = (float)ncount/graph[ins].degree;
				
			if(score != 0 && (network[ins].comm_count < 1 || score > (float)network[ins].dfbucket/buck)){
				//printf("%d\t", graph[ins].name);
				temp3 = (NODE *)malloc(sizeof(NODE));
				temp3 -> index = ins;
				temp3 -> phase = phase +1;
				temp3 -> next = NULL;
			 	temp3 -> ncount = ncount;
			 	
			 	i = 0;
				temp2 = network[list1].vertex -> next;
				while( i < graph[ins].degree && temp2 != NULL){
					j = graph[ins].link[i];
					if(j < temp2 -> index){
						if(j == network[list1].vertex -> index)
							network[list1].vertex ->ncount++;						
						i++;
					}
					else{
						if(j == temp2 -> index){
							temp2 -> ncount++;
							if( temp2 -> phase == phase + 1)
								temp3 -> ncount++;
							i++;
						}
						temp2 = temp2 -> next;
					}
				}
				network[list1].count++;
				
			 	temp1 -> next = temp3;
				temp1 = temp3;
			}
		}
		q++;
	}
}
