#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node{

	unsigned address;
	int dirty;

};

void LRU(FILE * trace, struct node array[], int amount);
void FIFO(FILE * trace, struct node array[], int amount);
void VMS(FILE * trace, int amount);

int main(int argc, char * argv[]){
	int amount = atoi(argv[2]);
	struct node array[amount];

	for(int i = 0; i < amount; i++){
		array[i].dirty = 0;
		array[i].address = 0;
	}

	FILE * trace;

	trace = fopen(argv[1], "r");

	if(trace == NULL)
		printf("error no such file found!\n");


	if(strcmp(argv[3], "LRU") == 0){
		LRU(trace, array, amount);
	}

	else if(strcmp(argv[3],"FIFO") == 0){
		//printf("fifo success! \n");
		FIFO(trace, array, amount);
	}

	else if(strcmp(argv[3], "VMS") == 0){
		VMS(trace, amount);

	}

	else{
		printf("Non-valid algorithm entered!\n");
		printf("Valid case sensitive arguments are: LRU, FIFO, VMS\n");
		exit(1);
	}

	return 0;
}

void LRU(FILE* trace,struct node array[], int amount){
	int reads = 0;
	int writes = 0;
	int i = 0;
	int track = 0;
	unsigned tempaddr;
	char tempc;
	int hitloc = 0;
	struct node temp;

	while(track != 1000000){

		fscanf(trace,"%x %c", &tempaddr, &tempc);
		tempaddr = tempaddr >> 12;

		//printf("%u, %c\n",tempaddr ,tempc);

		int hit = 0;

		for(int j = 0; j < amount; j++){
			if(tempaddr == array[j].address){
				//printf("address hit!\n");
				hit = 1;
				hitloc = j;
				temp = array[j];
				break;
			}
		}

		if(hit == 1){
			//do nothing
			//printf("hit\n");
				
				
				for(int k = 0; k < amount; k++){
				
					if(k >= i && k < hitloc){
						//do nothing
					}
					else
						array[k % amount] = array[(k+1) % amount];
				}

				array[(((i-1) + amount) % amount)] = temp;
		}
		else {
			reads++;
			if(array[i].dirty == 1)
				writes++;

			array[i].address = tempaddr;

			//printf("%u, %c\n",array[i].address ,array[i].rw);

			if(tempc == 'W')
				array[i].dirty = 1;
			else
				array[i].dirty = 0;
		
			if(i == amount)
				i = 0;
			else
				i++;
		}

		//printf("%c \n",array[i].rw );
		

		/*if(track%10000 == 0)
			printf("current %i reads, %i writes\n",reads ,writes );
		*/

		track++;
	}

	printf("total memory frames: %i\n",amount);
	printf("events in traces: %i\n",track);
	printf("total disk reads: %i\n",reads);
	printf("total disk writes: %i\n",writes);
}

void FIFO(FILE* trace, struct node array[], int amount){

	int reads = 0;
	int writes = 0;
	int i = 0;
	int track = 0;
	unsigned tempaddr;
	char tempc;

	while(track != 1000000){

		fscanf(trace,"%x %c", &tempaddr, &tempc);
		tempaddr = tempaddr >> 12;

		//printf("%u, %c\n",tempaddr ,tempc);

		int hit = 0;

		for(int j = 0; j < amount; j++){
			if(tempaddr == array[j].address){
				//printf("address hit!\n");
				hit = 1;
				break;
			}
		}

		if(hit == 1){
			//do nothing
			//printf("hit\n");
			
		}
		else {
			reads++;
			if(array[i].dirty == 1)
				writes++;

			array[i].address = tempaddr;

			//printf("%u, %c\n",array[i].address ,array[i].rw);

			if(tempc == 'W')
				array[i].dirty = 1;
			else
				array[i].dirty = 0;
		
			if(i == amount)
				i = 0;
			else
				i++;
		}

		//printf("%c \n",array[i].rw );
		

		/*if(track%10000 == 0)
			printf("current %i reads, %i writes\n",reads ,writes );
		*/

		track++;
	}

	printf("total memory frames: %i\n",amount);
	printf("events in traces: %i\n",track);
	printf("total disk reads: %i\n",reads);
	printf("total disk writes: %i\n",writes);

}

void VMS(FILE* trace, int amount){

	struct node p1[amount/2];
	struct node p2[amount/2];
	struct node dirty[(amount/2)+1];
	struct node clean[(amount/2)+1];
	unsigned memory[amount];
	unsigned tempaddr;
	char tempc;
	int track = 0;


	for(int i = 0; i < (amount/2); i++){
		p1[i].dirty = 0;
		p1[i].address = 0;
		p2[i].dirty = 0;
		p2[i].address = 0;
	}
	for(int i = 0; i < ((amount/2)+1); i++){
		dirty[i].dirty =1 ;
		dirty[i].address = 0;
		clean[i].dirty = 0;
		clean[i].address = 0;
	}

	

	while(track != 1000000){

		fscanf(trace,"%x %c", &tempaddr, &tempc);
		tempaddr = tempaddr >> 12;

		//if it falls in this range it belongs to p2
		if(tempaddr > 0x2AAAA && tempaddr < 0x40000){

			for(int i = 0; i <(amount/2);i++){
				if(tempaddr == p2[i].address){
					//do nothing
				}
				else{
					
				}
			}



		}

		//
		else{


		}


		track++;
	}

}