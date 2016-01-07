extern int buck;
void delete_community(CLUSTER **head, CLUSTER *network, int index, int move){

	int j;
	NODE *temp1, *temp3;

	temp1 = network[index].vertex -> next;
	if(move == 1){
		while (temp1 != NULL){
			temp3 = temp1;			
			if(temp3 -> score){
			   network[temp3 -> index].comm_count--;
			   
			   j = (temp3 -> score - 0.01)*buck;
			   network[temp3 -> index].bcount[j]--;
			   
			   j = (temp3 -> df - 0.01)*buck;
			   network[temp3 -> index].dfbcount[j]--;
			}
			temp1 = temp1 -> next;
			free(temp3);
		}
	}
	else{
		while (temp1 != NULL){
			temp3 = temp1;
			temp1 = temp1->next;
			free(temp3);
		}
	}
	if(network[index].prev != NULL)						
		network[index].prev -> next = network[index].next;
	else
		*head = network[index].next;
	if(network[index].next != NULL)
		network[index].next -> prev = network[index].prev;
	network[index].vertex = NULL;	
	network[index].prev = NULL;
	return;
}
