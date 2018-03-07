#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node{

	unsigned address;
	int dirty;
   int time_since_last_use;
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
      if (strcmp(argv[1], "gcc.trace") != 0){
         printf("Error: Attempted to call VMS with non-gcc trace!  Aborting.\n");
         exit(1);
      }
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
	int track = 0;
	char tempc;
	

	while(track != 1000000){

		struct node new_page;
		fscanf(trace,"%x %c", &new_page.address, &tempc);
		new_page.address = new_page.address >> 12;

		
		int hit = 0;

		//check to see if the page is already loded
		int j = 0;
		for(;j < amount; j++){
			if(new_page.address == array[j].address){
	
				hit = 1;
				break;
			}
		}

		//if page is loaded reset the timer to 0
		if(hit == 1){
			
				array[j].time_since_last_use = 0;
				for(int i = 0; i < amount; i++){
					if(i != j)
						array[i].time_since_last_use++;
				}

		}

		//if the page isn't loaded add it and increment all the old entries timers
		else {
			reads++;


			int oldest = 0;
			int loc = 0;

			for(int j = 0; j < amount; j++){

				if(array[j].time_since_last_use > oldest){
					oldest = array[j].time_since_last_use;
					loc = j;
				}

			}

			if(array[loc].dirty == 1)
				writes++;

			array[loc].address = new_page.address;
			array[loc].time_since_last_use = 0;

			for(int i = 0; i < amount; i++){
				if(i != loc)
					array[i].time_since_last_use++;
			}


			if(tempc == 'W')
				array[loc].dirty = 1;
			else
				array[loc].dirty = 0;
			
		}
		

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
	char tempc;

	while(track != 1000000){

		struct node new_page;
		fscanf(trace,"%x %c", &new_page.address, &tempc);
		new_page.address = new_page.address >> 12;

		

		int hit = 0;
		for(int j = 0; j < amount; j++){
			if(new_page.address == array[j].address){
				//printf("address hit!\n");
				hit = 1;
				break;
			}
		}

		if(hit == 1){
			//do nothing
			//printf("hit\n");
			
		} else {
			reads++;

			if(array[i].dirty == 1)
				writes++;

			array[i].address = new_page.address;

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
	char tempc;
	int track = 0;
   int p1_indicator_slot = 0;
   int p2_indicator_slot = 0;
   int dirty_indicator_slot = 0;
   int clean_indicator_slot = 0;
   int writes = 0;
   int reads = 0;

	for(int i = 0; i < (amount / 2); i++){
		p1[i].dirty       = -1;
		p1[i].address     = -1;
		p2[i].dirty       = -1;
		p2[i].address     = -1;
	}
	for(int i = 0; i < ((amount / 2) + 1); i++){
		dirty[i].dirty    = -1;
		dirty[i].address  = -1;
		clean[i].dirty    = -1;
		clean[i].address  = -1;
	}
   for (int i = 0; i < amount; i++)
      memory[i] = -1;
	

	while(track != 1000000){

      // Get the page from the address
      struct node new_page;
		fscanf(trace,"%x %c", &new_page.address, &tempc);
		new_page.address = new_page.address >> 12;
      if (tempc == 'R')
         new_page.dirty = 0;
      else if (tempc == 'W')
         new_page.dirty = 1;
      else
         printf("PANIC R/W\n");

      int hit = 0;
      struct node evicted_page;

		// Address was called by p1
		if(new_page.address > 0x2FFFF && new_page.address < 0x40000){
			for(int i = 0; i < (amount / 2); i++){
				if(new_page.address == p1[i].address)
					hit = 1;
               break;
			}
         if (hit == 0){
         // Handle the page fault on p1 (save the current eviction target and place new page there, cycle p1 indicator)
            evicted_page = p1[p1_indicator_slot];
            p1[p1_indicator_slot] = new_page;
            p1_indicator_slot++;
            if (p1_indicator_slot >= amount / 2)
               p1_indicator_slot = 0;
         }
		} 

      // Address was called by P2
      else {
			for(int i = 0; i < (amount / 2); i++){
				if(new_page.address == p2[i].address)
					hit = 1;
               break;
			}
         if (hit == 0){
         // Handle the page fault on p2 (save the current eviction target and place new page there, cycle p2 indicator)
            evicted_page = p2[p2_indicator_slot];
            p2[p2_indicator_slot] = new_page;
            p2_indicator_slot++;
            if (p2_indicator_slot >= amount / 2)
               p2_indicator_slot = 0;

         }
      }




      // If we evicted a page, send it to clean or dirty
      if (evicted_page.address != -1){
         if (evicted_page.dirty == 0){
            clean[clean_indicator_slot] = evicted_page;
            clean_indicator_slot++;
            if (clean_indicator_slot >= amount / 2 + 1)
               clean_indicator_slot = 0;
         }
         else if (evicted_page.dirty == 1){
            dirty[dirty_indicator_slot] = evicted_page;
            dirty_indicator_slot++;
            if (dirty_indicator_slot >= amount / 2 + 1)
               dirty_indicator_slot = 0;
         }
         else
            printf("PANIC invalid dirty value\n");
      }


      // If the new page is already in memory, it must be in clean/dirty, remove it from Clean/Dirty
         hit = 0;
         for (int i = 0; i < amount; i++){
            if (new_page.address == memory[i]){
               hit = 1;
               break;
            }
         }
         // If we found the page in memory, scan dirty/clean and remove
         if (hit == 1){
            for (int i = 0; i < amount / 2 + 1; i++){
               // WE MIGHT NEED TO MOVE ALL ARRAY ENTRIES BELOW UP A SLOT------------------------
               if (new_page.address == clean[i].address){
                  clean[i].address = -1;
                  clean[i].dirty= -1;
                  break;
               }
               if (new_page.address == dirty[i].address){
                  dirty[i].address = -1;
                  dirty[i].dirty= -1;
                  writes++;
                  break;
               }
            }
         }
         // If not in memory, put into memory if free space, otherwise seek a page from Clean/Dirty to remove to get space
         else {         
            hit = 0;
            for (int i = 0; i < amount; i++){
               if (memory[i] == -1){
                  memory[i] = new_page.address;
                  hit = 1;
               } 
            }

            int is_done = 0;
            if (hit == 0){
               // Scan through Clean for a page to free from memory, then Dirty
               for (int i = 0; i < amount / 2 + 1; i++){
                  if (clean[i].address != -1){
                     // Found a page to remove.  Remove it from memory and add new_page there, eliminate here
                     for (int j = 0; j < amount; j++){
                        if (memory[j] == clean[i].address){
                           memory[j] = new_page.address;
                           clean[i].address = -1;
                           clean[i].dirty= -1;
                           is_done = 1;
                           break;
                        }
                     }
                  }
               }

               if (is_done == 0){
                  for (int i = 0; i < amount / 2 + 1; i++){
                     if (dirty[i].address != -1){
                        // Found a page to remove.  Remove it from memory and add new_page there, eliminate here
                        for (int j = 0; j < amount; j++){
                           if (memory[j] == dirty[i].address){
                              memory[j] = new_page.address;
                              dirty[i].address = -1;
                              dirty[i].dirty= -1;
                              break;
                           }
                        }
                     }
                  }
               }
            }
         }
      

		track++;
	}

	printf("total memory frames: %i\n",amount);
	printf("events in traces: %i\n",track);
	printf("total disk reads: %i\n",reads);
	printf("total disk writes: %i\n",writes);

}
