#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <math.h>

typedef struct Blocks{
	unsigned long int value;
	struct Blocks *next;
	int nrubit;
	
}Block;



//Insert Function Prototypes Here
unsigned long int sizeLL(struct Blocks *); 
int searchLL(struct Blocks *, unsigned long int);
void addtoback(struct Blocks*, unsigned long int);
int checknru(struct Blocks *);
void makenru(struct Blocks *, unsigned long int);
void insertnru(struct Blocks *, unsigned long int);
void movetoback(struct Blocks*, unsigned long int);
void addtofront(struct Blocks *, unsigned long int);
void movetofront(struct Blocks *, unsigned long int);

int main(int argc, char* argv[]){
	if (argc != 6){
		printf("Error: Incorrect Number of Arguments\n");
		return 1;
	}
	
	//Check Cache Size must be power of 2
	int cachesize = atoi(argv[1]);
	if (cachesize == 0 || cachesize == 1){
		printf("Error: Invalid Cache Size\n");
		return 1;
	} else {
		int temp = cachesize;
		while(temp != 1){
			if (temp%2 != 0){
				printf("Error: Invalid Cache Size\n");
			      	return 1;
			}
			temp = temp/2;
		}
	}
	 
	
	//Check Block Size must be a power of 2
	int blocksize = atoi(argv[4]);
	if (blocksize == 0){
		printf("Error: Invalid Block Size\n");
		return 1;
	} else {
		int temp = blocksize;
		while (temp != 1){
			if (temp%2 != 0){
				printf("Error: Invalid Block Size\n");
				return 1;
			}
			temp = temp/2;
		}
	}
	
	
	//Check for proper cache replacement policy
	int temp1 = strcmp(argv[3],"fifo");
	int temp2 = strcmp(argv[3], "lru");
	int temp3 = strcmp(argv[3], "nru");
	
	int fifo = 0;
	int lru = 0;
	int nru = 0;


	if (temp1 == 0){
		fifo = 1;
	} else if (temp2 == 0){
		lru = 1;
	} else if (temp3 == 0){
		nru = 1;
	} else {
		printf("Error: Invalid Cache Replacement Policy\n");
		return 1;
	}
	
	
	//Check Associativity
	
	int assoc = 0;
	int nassoc = 0;
	int direct = 0;

	
	int tmp1 = strcmp(argv[2], "direct");
	int tmp2 = strcmp(argv[2], "assoc");
	
	int n;
	char *nway = strstr(argv[2], "assoc:");
	if (nway){
		nassoc = 1;
		n = atoi(argv[2]+6);
	} else if (tmp1 == 0){
		direct = 1;
		n = 1;
	} else if (tmp2 == 0){
		assoc = 1;
	} else {
		printf("Error: Invalid Associativity\n");
		return 1;
	} 
	
	
	
	//File Checking 
	FILE *fp = fopen(argv[5], "r");
	if (fp == NULL){
		printf("Error: File Doesn't Exist\n");
		return 1;
	}
	
	//done with input errors
	
	unsigned long int read = 0;
	unsigned long int write = 0;
	unsigned long int hit = 0;
	unsigned long int miss = 0;



	//BEGINNING OF PROJECT
	
	//Determine number of blocks and sets
	unsigned long int blocks = 0;
	unsigned long int sets = 0; 
	if (assoc == 1){
		blocks = cachesize/blocksize;
		sets =  1; 
	} else if (nassoc == 1){ 
		blocks = n;
		sets = cachesize/(blocksize*n);
	} else if (direct == 1){
		blocks = 1;
		sets = cachesize/blocksize; 
	}

	unsigned long int programcounter;
	char command;
	unsigned long int address;

	//Beginning of Cache stuff
		
	unsigned long int block_bits = log(blocksize)/log(2);
	unsigned long int index_bits = log(sets)/log(2);
	unsigned long int mask_index = ((pow(2,index_bits))-1);

	if (direct == 1){
		struct Blocks table[sets];	
		while(fscanf(fp, "%lx: %c %lx", &programcounter, &command, &address)){
			unsigned long int index = (address >> block_bits) & mask_index;
			unsigned long int tag = (address >> (block_bits + index_bits));
			if(command == 'R'){	
				if(table[index].value == tag){
					hit++;
				} else {
					table[index].value = tag;
					miss++;
					read++;
				}			
			} else if (command == 'W'){
				if(table[index].value == tag){
					hit++;
					write++;
				} else {
					table[index].value = tag;
					miss++;
					read++;
					write++;
				}	
			}
		}
	} else if (assoc == 1){
		int addfirst = 0;
		struct Blocks *head = (struct Blocks *)malloc(sizeof(struct Blocks));
		
		while(fscanf(fp, "%lx: %c %lx", &programcounter, &command, &address)){
			unsigned long int tag = (address >> block_bits);
				if (fifo == 1){//FIFO
					if (addfirst == 0){
						addfirst = 1;
						if (command == 'R'){
							miss++;
							read++;
							head->value = tag;	
						} else {
							miss++;
							read++;
							write++;
							head->value = tag;
						}
 					}else
						 if(command == 'R'){
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;	
							} else {
								miss++;
								read++;
								addtoback(head,tag);
								head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
							} else {
								miss++;
								read++;
								addtoback(head,tag);
							}
						}
					} else {
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;
								write++;	
							} else {
								miss++;
								read++;	
								write++;
								addtoback(head,tag);
								head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								write++;
							} else {
								miss++;
								read++;
								write++;
								addtoback(head,tag);
							}
						}

					}
				} else if (lru == 1){//LRU



					if (addfirst == 0){
						addfirst = 1;
						if (command == 'R'){
							miss++;
							read++;
							head->value = tag;	
						} else {
							miss++;
							read++;
							write++;
							head->value = tag;
						}
 					}else if(command == 'R'){
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;	
								//move value to front
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								//addfrontremovetail(head,tag);
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
								struct Blocks *prev = head;
								struct Blocks *ptr = head->next;
								while(ptr->next != NULL){
									prev = prev->next;
									ptr = ptr->next;
								}
								prev->next = NULL;
								//head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
							}
						}
					} else {//command = W
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;
								write++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}	
							} else {
								miss++;
								read++;	
								write++;
								//addfrontremovetail(head,tag);
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
								struct Blocks *prev = head;
								struct Blocks *ptr = head->next;
								while(ptr->next != NULL){
									prev = prev->next;
									ptr = ptr->next;
								}
								prev->next = NULL;
								//head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								write++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								write++;
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
							}
						}

					}







				} else if (nru == 1){//NRU
					if (addfirst == 0){
						addfirst = 1;
						miss++;
						read++;
						head->value = tag;
						head->nrubit = 0;
						head->next = NULL;	
						if(command == 'W') {
							write++;
						}
 					}else if(searchLL(head,tag) == 1){
						hit++;
						if(command == 'W'){
							write++;
						}
					} else {	
						miss++;
						read++;
						if(command == 'W'){
							write++;
						}
						if(sizeLL(head) == blocks){
							if (checknru(head) == 0){
								makenru(head,tag);//gets rid of old head automatically
							}else{
								insertnru(head,tag);
							}
						} else {
							addtoback(head,tag);
							
						}
					
					}
	
				}
		}//end while
	
	} else if (nassoc == 1){
		//struct Blocks* table[sets];
		unsigned long int i;
		for(i = 0; i < sets; i++){
			int addfirst = 0;
			struct Blocks *head = (struct Blocks *)malloc(sizeof(struct Blocks));
			//table[i]=head;
		
		while(fscanf(fp, "%lx: %c %lx", &programcounter, &command, &address)){
			unsigned long int tag = (address >> (block_bits+index_bits));
			unsigned long int index = (address >> block_bits) & mask_index;
			if(index == i){
				if (fifo == 1){//FIFO
					if (addfirst == 0){
						addfirst = 1;
						if (command == 'R'){
							miss++;
							read++;
							head->value = tag;	
						} else {
							miss++;
							read++;
							write++;
							head->value = tag;
						}
 					}else
						 if(command == 'R'){
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;	
							} else {
								miss++;
								read++;
								addtoback(head,tag);
								head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
							} else {
								miss++;
								read++;
								addtoback(head,tag);
							}
						}
					} else {
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;
								write++;	
							} else {
								miss++;
								read++;	
								write++;
								addtoback(head,tag);
								head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								write++;
							} else {
								miss++;
								read++;
								write++;
								addtoback(head,tag);
							}
						}

					}
				} else if (lru == 1){//LRU
					


					

					if (addfirst == 0){
						addfirst = 1;
						if (command == 'R'){
							miss++;
							read++;
							head->value = tag;	
						} else {
							miss++;
							read++;
							write++;
							head->value = tag;
						}
 					}else if(command == 'R'){
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;	
								//move value to front
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								//addfrontremovetail(head,tag);
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
								struct Blocks *prev = head;
								struct Blocks *ptr = head->next;
								while(ptr->next != NULL){
									prev = prev->next;
									ptr = ptr->next;
								}
								prev->next = NULL;
								//head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
							}
						}
					} else {//command = W
						if (sizeLL(head) == blocks){
							if(searchLL(head, tag) == 1){
								hit++;
								write++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}	
							} else {
								miss++;
								read++;	
								write++;
								//addfrontremovetail(head,tag);
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
								struct Blocks *prev = head;
								struct Blocks *ptr = head->next;
								while(ptr->next != NULL){
									prev = prev->next;
									ptr = ptr->next;
								}
								prev->next = NULL;
								//head = head->next;
							}
						} else {
							if(searchLL(head,tag) == 1){
								hit++;
								write++;
								if(head->value != tag){
									//traverse till you find it
									struct Blocks *ptr = head->next;
									struct Blocks *prev = head;
									while(ptr->value != tag){
										prev = prev->next;
										ptr = ptr->next;	
									}
									prev->next = ptr->next;
									struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
									temp->value = tag;
									temp->next = head;
									head = temp;
								}
							} else {
								miss++;
								read++;
								write++;
								struct Blocks *temp = (struct Blocks *)malloc(sizeof(struct Blocks));
								temp->value = tag;
								temp->next = head;
								head = temp;
							}
						}

					}







				} else if (nru == 1){//NRU
					if (addfirst == 0){
						addfirst = 1;
						miss++;
						read++;
						head->value = tag;
						head->nrubit = 0;
						head->next = NULL;	
						if(command == 'W') {
							write++;
						}
 					}else if(searchLL(head,tag) == 1){
						hit++;
						if(command == 'W'){
							write++;
						}
					} else {	
						miss++;
						read++;
						if(command == 'W'){
							write++;
						}
						if(sizeLL(head) == blocks){
							if (checknru(head) == 0){
								makenru(head,tag);//gets rid of old head automatically
							}else{
								insertnru(head,tag);
							}
						} else {
							addtoback(head,tag);
							
						}
					
					}
				}
			}
		}//end while
			rewind(fp);	
		}
			
	}//end nassoc
	
	fclose(fp);
	
	printf("Memory reads: %lu\nMemory writes: %lu\nCache hits: %lu\nCache misses: %lu\n",read,write,hit,miss);
	return 0;	

}//end main

unsigned long int sizeLL(struct Blocks *head){
	struct Blocks *ptr = head;
	unsigned long int counter = 0;
	while(ptr != NULL){
		ptr = ptr->next;
		counter++;
	}
	return counter;
}

int searchLL(struct Blocks *head, unsigned long int tag){
	struct Blocks *ptr = head;
	while (ptr != NULL){
		if (ptr->value == tag){
			ptr->nrubit = 0;
			return 1;
		}
		ptr = ptr->next;
	}
	return 0;
}

void addtoback(struct Blocks *head, unsigned long int tag){
	struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
	temp->nrubit = 0;
	temp->value = tag;
	temp->next = NULL;
	if(head->next == NULL){
		head->next = temp;
		return;
	}
	struct Blocks *ptr = head;
	while(ptr->next != NULL){
		//printf("problem\n");
		ptr = ptr->next;
	}
	ptr->next = temp;
	
}

int checknru(struct Blocks *head){//returns 0 if all nrubit values are 0 
	struct Blocks *ptr = head;
	while (ptr != NULL){
		if (ptr->nrubit == 1){
			return 1;
		}
		ptr = ptr->next;
	}
	return 0;
}

void makenru(struct Blocks *head, unsigned long int tag){
	//make all NRU bits 1 and insert at head with new node NRU bit 0
	struct Blocks *ptr = head;
	while (ptr != NULL){
		ptr->nrubit = 1;
		ptr = ptr->next;
	}
	head->value = tag;
	head->nrubit = 0;
}

void insertnru(struct Blocks *head, unsigned long int tag){
	struct Blocks *ptr = head;
	while(ptr != NULL){
		if(ptr->nrubit == 1){
			ptr->nrubit = 0;	
			ptr->value = tag;
			return;
		}
		ptr = ptr->next;
	}
	
}

void movetoback(struct Blocks *head, unsigned long int tag){
	struct Blocks *ptr = head;
	struct Blocks *prev = ptr;
	while(ptr->value != tag){
		prev=ptr;
		ptr=ptr->next;
	}
	struct Blocks *temp = ptr;
	prev->next = ptr->next;
	while(ptr->next != NULL){
		ptr = ptr->next;
	}
	ptr->next = temp;
	temp->next = NULL;
}

void addtofront(struct Blocks* head, unsigned long int tag){
	struct Blocks *temp = (struct Blocks *) malloc(sizeof(struct Blocks));
	temp->value = tag;
	temp->nrubit = 0;
	temp->next = head;
	head = temp;
}


void movetofront(struct Blocks *head, unsigned long int tag){
	if(head->value == tag){
		return;
	}
	struct Blocks *ptr = head->next;
	struct Blocks *prev = head;
	while(ptr->value != tag){
		ptr = ptr->next;
		prev = prev->next;
	}
	prev->next = ptr->next;
	ptr->next = head;
	head = ptr;
}
	
