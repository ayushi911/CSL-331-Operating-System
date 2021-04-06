#include <stdint.h>
#include<stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include<sys/time.h>


uint16_t xs;

uint16_t xorshift(void) 
{   
    xs ^= xs << 7;
    xs ^= xs >> 9;
    xs ^= xs << 8;
    return xs;
}
struct PageTableEntry{
	unsigned int pfn : 14;	// Physical Frame Number
	unsigned int valid : 1;	// Valid
	unsigned int present : 1;	// Present
	unsigned int protect : 3;	// Protect 01R, 10W, 11X
	unsigned int dirty : 1;	// Dirty
	unsigned int supervisor : 1;	// User/Kernel
};

#define PAGES 64 

char *dec2bin(int n)
{
  int c, d, t;
  char *p;

  t = 0;
  p = (char*)malloc(24+1);

  if (p == NULL)
    exit(EXIT_FAILURE);

  for (c = 23 ; c >= 0 ; c--)
  {
    d = n >> c;

    if (d & 1)
      *(p+t) = 1 + '0';
    else
      *(p+t) = 0 + '0';

    t++;
  }
  *(p+t) = '\0';

  return  p;
}

int initPageTable(struct PageTableEntry *PTE){

	srand(time(NULL));
	PTE[0].pfn = xorshift() % 16384;
	PTE[0].valid = 1;
	PTE[0].present = xorshift() % 2;
	PTE[0].protect = 7;
	PTE[0].dirty = xorshift() % 2;
	PTE[0].supervisor = xorshift() % 2;

	for (int i = 1; i < PAGES; ++i){
		srand(time(NULL));

		int validPFN_Flag = 0;

		while (!validPFN_Flag){
			PTE[i].pfn = xorshift() % 16384;

			for (int j = 0; j < i; ++j){
				if (PTE[i].pfn == PTE[j].pfn){
					validPFN_Flag = 0;
					break;
				} else{
					validPFN_Flag = 1;
					PTE[i].valid = 1;
					PTE[i].present = xorshift() % 2;
					PTE[i].protect =7;
					PTE[i].dirty = xorshift() % 2;
					PTE[i].supervisor = xorshift() % 2;

					sleep(0.001);
				}
			}
		}
	}
	for (int i=0;i<2;i++){
		int j=xorshift()%64;
		PTE[j].valid=xorshift()%2;
	}
	for (int i=0;i<2;i++){
		int j=xorshift()%64;
		PTE[j].protect=xorshift()%8;
	}

	return 1;
}

int get_VPN(int request){
	int mask = 0xFC00; // [(111111)-0000000000]
	int VPN = (request & mask) >> 10;
	return VPN;
}

int get_Offset(int request){
	int mask = 0x3FF; // [(000000)-1111111111]
	return (request & mask);
}

int AccessMemory(int request, struct PageTableEntry *PTE){
	int VPN = get_VPN(request);
	int offset = get_Offset(request);
	struct PageTableEntry found=PTE[VPN];
	printf("\tVPN :%d",VPN);
	printf("\t\toffset :%d",offset);
	if (found.valid==0){
		printf("\tSEGMENTATION_FAULT");
		return -1;
	}
	else if (found.protect < 1){
		printf("\tPROTECTION_FAULT");
		return -1;
	}
	else{
		int physical_address = (found.pfn << 10)|offset;
		printf("\tPhysical Address  :%s", dec2bin(physical_address));
		return physical_address;
	}
	//PTE[VPN].offset = offset;
}


int main(int argc, char *argv[]){
	struct PageTableEntry PTE[PAGES]; // 64 pages per process
	srand(time(0));
    xs=rand();
	initPageTable(PTE);
	printf("\n======================================================================\n");
        printf("PAGE NUMBER\tPFN\tVALID\tPRESENT\tPROTECT\tDIRTY\tSUPERVISOR \n");
        printf("======================================================================\n");
        printf("\n\n");
	for (int i = 0; i < PAGES; ++i){
		printf("%d\t\t%d\t  %d\t  %d\t  %d\t  %d\t  %d\n", i, PTE[i].pfn, PTE[i].valid,PTE[i].present, PTE[i].protect, PTE[i].dirty, PTE[i].supervisor );

		// printf("PAGE NUMBER %d\n", i);
		// printf("\tPFN %d\n", PTE[i].pfn);
		// printf("\tvalid %d\n", PTE[i].valid);
		// printf("\tpresent %d\n", PTE[i].present);
		// printf("\tprotect %d\n", PTE[i].protect);
		// printf("\tdirty %d\n", PTE[i].dirty);
		// printf("\tsupervisor %d\n", PTE[i].supervisor);
	}
///address for translation
	int request;
	int sequences[4]={25,50,75,100};
	for (int i=0;i<4;i++){//change it before running!!
	printf("\n=================================FOR SEQUENCE OF %d=================================\n",sequences[i]);
				double overall_time=0;
				int correct=0;
		for (int j=0;j<10;j++){
			    struct timeval  start, end;
			for (int k=0;k<sequences[i];k++){
				//printf("\n=================================Loop running for %d times=================================\n",sequences[j]);
				double time;
				request=xorshift()%65536;
				printf(" Virtual address :%s ",dec2bin(request));
				gettimeofday(&start, NULL);
				int success = AccessMemory(request, PTE);
    			gettimeofday(&end, NULL);
    			time=(double)(end.tv_usec - start.tv_usec) / 1000000 + (end.tv_sec - start.tv_sec);
				printf("\t Time taken is %f\n",time);
				if (success==-1){
					break;
				}
				else{
					correct=correct+1;
					overall_time=overall_time+time;
				}
				
			}
		}
		printf("\n==============================================\n");
        printf("||Average time taken in this trial %f ||\n",overall_time/correct);
        printf("==============================================\n");
		
	}
	

	return 0;
}