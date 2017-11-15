#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <semaphore.h>
#define Max_No_of_Threads 10
#define MAX_MEM_SIZE 32
#define Max_Thread_Count 5

//Stucture for threads
struct thread_Data
{
  int id;
  int size;
};

//Structure for memory block
struct Memory_Block
{
  char *blockStartAddress;
  int blockSize;
  int dataInBlock;
};


// global variable, all threads can acess
void *allocatedAddress;
char *StartAddr;
char *EndAddr;
thread_Data Threads[Max_No_of_Threads];
Memory_Block Blocks[Max_Thread_Count];
pthread_mutex_t Mutex;
int sw,No_of_Threads,block_Count,ThreadIndex;
sem_t bin_sem; // semaphore
pthread_mutex_t mutx; // mutex
void InitializeBlocks();
void Print_Memory();
void *MMS(void *arg);
char *ReqMem(int size);
char *FirstFit(int blockAllocationSize, int threadIndex);
char *BestFit(int blockAllocationSize, int threadIndex);
char *WorstFit(int blockAllocationSize, int threadIndex);

int main(int argc, char *argv[])
{
  int i;
  allocatedAddress = malloc(MAX_MEM_SIZE);
  StartAddr = (char*)allocatedAddress;
  InitializeBlocks();
  No_of_Threads = atoi(argv[1]);
  sw = atoi(argv[2]);
  int state1, state2;
  state1 = pthread_mutex_init(&mutx, NULL);
  state2 = sem_init(&bin_sem, 0 ,0);
  if(state1 || state2 !=0)
    puts("Error mutex & semaphore initialization!!!");
  for (i = 1; i <= No_of_Threads; i++)
    {
      pthread_t id;
      pthread_create(&id, NULL, MMS, NULL);
    }
  sleep(2);
  free(allocatedAddress);
  return 0;
  
}

void *MMS(void *arg)
{
  char *allocAddr,*pointtoAddr,*blockAddress;
  int p,memReq, i, j,blkAlloc=2;
  blockAddress = StartAddr;
  Threads[ThreadIndex].id = (int)pthread_self();
  //printf("I am thread %d going to sleep\n",Threads[ThreadIndex]);
  p = 1 +(rand() % 10);
    Threads[ThreadIndex].size = p;
 
  pthread_mutex_lock(&mutx);
  //sleep(1);
  //printf("I am thread %d waking up\n",Threads[ThreadIndex]);
  //locking the mutex for the thread
  {
    switch(sw)
      {
      case 1:{
	allocAddr = FirstFit(p, ThreadIndex);
	printf("Starting address %d and memory requested is %d\n",allocAddr,p);
	if(allocAddr == 0)
	printf("Insufficient Memory Error!!!\n");
	break;
      }
      case 2:
	{
	  allocAddr = BestFit(p, ThreadIndex);
	  printf("Starting address %d and memory requested is %d\n",allocAddr,p);
          if(allocAddr == 0)
	  printf("Insufficient Memory Error!!!\n");
	  break;
	}
      case 3:{
	  allocAddr = WorstFit(p, ThreadIndex);
	  printf("Starting address %d and memory requested is %d\n",allocAddr,p);
          if(allocAddr == 0)
          printf("Insufficient Memory Error!!!\n");
	  break;
      }
      default:{
	printf("You selected invalid input");
	exit(-1);
      }
      }
    if(allocAddr == 0)
    goto UNLOCK;
    pointtoAddr = allocAddr;
    printf("Block Allocated=%d\n", p);
    for (i = 0;i < p;i++)
      {
	*pointtoAddr = (ThreadIndex + 1);
	pointtoAddr++;
      }
     printf("Ending address %d\n",pointtoAddr);

    //Memory Info
    for(i=0; i < block_Count; i++)
	{
		for(j=0; j < Blocks[i].blockSize; j++){		
                      if (*Blocks[i].blockStartAddress != 0){
				Blocks[i].dataInBlock = 1;
				break;
			}	
		}
	}
    printf("\nALLOCATED MEMORY UNIT\n");
    Print_Memory();
    ThreadIndex++;
    UNLOCK: pthread_mutex_unlock(&mutx);
    //unlocking the mutex
  }
  
  
  
}

// First Fit Algorithm
char *FirstFit(int blockAllocationSize, int threadIndex)
{
  char *pointer;
  int i, found = 0;
  printf("\nFirst Fit\n");
  for (i = 0;i < block_Count;i++)
    {
      if ((Blocks[i].dataInBlock == 0) && (Blocks[i].blockSize >= blockAllocationSize))
	{
	  found = 1;
	  break;
	}
    }
  if (found == 0)
    {
      pointer = ReqMem(blockAllocationSize);
    }
  else
    pointer = Blocks[i].blockStartAddress;
  return pointer;
}

//Best Fit Algorithm
char *BestFit(int blockAllocationSize, int threadIndex)
{
  printf("entered best fit\n");
  printf("memory requested is %d \n",blockAllocationSize);
  char *pointer;
  int i = 0, j=0, found = 0;
  int t;
  int required;
  required = blockAllocationSize;
  
  printf("No of blocks is %d\n",block_Count);
  while(i < block_Count)
  {
    //printf("entered while loop \n");
    for(j=0; j < block_Count; j++)
	{
		//printf("entered for loop \n");
		if((required == Blocks[j].blockSize) && (Blocks[j].dataInBlock == 0))
		goto EXIT;
	}
	required++;
	i++;
  } 
  EXIT: pointer = Blocks[j].blockStartAddress;
  return pointer;
}

//Worst Fit Algorithm
char *WorstFit(int blockAllocationSize, int threadIndex)
{
  char *pointer;
  int i = 0, j, found = 0,max;
  int t;
  
  for(i=0;i < block_Count;i++)
  {
	if(Blocks[i].dataInBlock == 0)
	{
		max = Blocks[i].blockSize;
		break;		
	}	
  } 
  if((Blocks[0].dataInBlock == 0) && (blockAllocationSize <= Blocks[0].blockSize))
  {
  pointer = Blocks[0].blockStartAddress;
  found = 1;
  }
  for(i=1; i < block_Count; i++)
  {
	if((max <= Blocks[i].blockSize) && (Blocks[i].dataInBlock == 0))
		{
			if(blockAllocationSize <= Blocks[i].blockSize)
			{			
			max = Blocks[i].blockSize;
			pointer = Blocks[i].blockStartAddress;
			found = 1;
			}
		}
  }

  if(found == 1)
  return pointer;
  else 
  return 0;
}

//Initialize Blocks
void InitializeBlocks()
{
  char *temp,*var;
  int i,n,j,k,p;
  p=0;
  k=0;
  var = StartAddr;
  temp = StartAddr;
  for (i = 0;i <Max_Thread_Count ;i++)
    {
      //Blocks[i].blockSize = 0;
      Blocks[i].dataInBlock = 0;
      //Blocks[i].blockStartAddress = StartAddr;
    }
  for (i = 0;i < MAX_MEM_SIZE;i++)
    {
      *temp = 0;
      temp++;
    }
  EndAddr = (temp);
  i =0;
  while(p < MAX_MEM_SIZE)
  { 
	n = 1+rand()%8;
	k = k+n;
	if(k > MAX_MEM_SIZE)
	{
		n = MAX_MEM_SIZE-p;
	}
	p = p + n;
	
	if(i == 0)
	{
		Blocks[i].blockSize = n;
		printf("Block size for block %d is %d\n",i,Blocks[i].blockSize);
		Blocks[i].blockStartAddress = StartAddr;
		printf("Starting address is for block %d is %d\n", i,Blocks[i].blockStartAddress);
		for(j=0; j < n; j++)
		{
			var++;
		}
	}
	else
	{
		Blocks[i].blockSize = n;
		printf("Block size for block %d is %d\n",i,Blocks[i].blockSize);
		Blocks[i].blockStartAddress = var;
		printf("Starting address for block %d is %d\n", i,Blocks[i].blockStartAddress);
		for(j=0;j < n; j++)
		{
			var++;
		}
	}
	i++;
  }
  block_Count = i;
}


//Request Memory from MMU
char *ReqMem(int size)
{
  int i;
  int found = 0;
  for (i = 0;i < block_Count;i++)
    {
      if ((Blocks[i].dataInBlock == 0) && (Blocks[i].blockSize >= size)){
	found =1;	
	break;
	}
    }
  if (found == 1)
  return Blocks[i].blockStartAddress;
  else if(found == 0)
	{ 
		return 0;
	}
}

// Display Memory Index
void Print_Memory()
{
  int i;
  char *index;
  index = StartAddr;
  printf("\n--------MEMORY BLOCK---------\n");
  //printf("Starting address: %d\n", index);
  for (i = 0;i < MAX_MEM_SIZE;i++)
    {
      printf("%d\t\t\t", *index);
      index++;
    }
   //printf("Ending  address: %d\n", index);
  printf("\nNumber of Blocks = %d\n\n", block_Count);
}


