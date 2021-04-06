#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>

#define PAGES 64
#define TLB_SIZE 32
int last=0;
uint16_t xs;

/////RANDOM ADDRESS GENERATOR
uint16_t xorshift(void) 
{   
    xs ^= xs << 7;
    xs ^= xs >> 9;
    xs ^= xs << 8;
    return xs;
}
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
struct PageTableEntry{
	unsigned int pfn : 14;	
	unsigned int valid : 1;	
	unsigned int present : 1;	
	unsigned int protect : 3;	
	unsigned int dirty : 1;	
	unsigned int supervisor : 1;
};

struct TLB_Entry{
	unsigned int active : 1;	
	unsigned int vpn : 6;	
	unsigned int pfn : 14;	// Physical Frame Number
	unsigned int valid : 1;	// Valid
	unsigned int present : 1;	// Present
	unsigned int protect : 3;	// Protect 01R, 10W, 11X
	unsigned int dirty : 1;	// Dirty
	unsigned int supervisor : 1;	
};
	struct TLB_Entry TLBE[TLB_SIZE];



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

int get_VPN(long int request){
	int mask = 0xFC00; // [(111111)-0000000000]

	int VPN = (request & mask) >> 10;

	return VPN;
}

int get_Offset(long int request){
	int mask = 0x3FF; // [(000000)-1111111111]

	return (request & mask);
}
int TLB_Lookup(int VPN,int tlb){
	int ans=-1;
	for (int i=0;i<tlb;i++){
		if((TLBE[i].vpn)==VPN){
			TLBE[i].active=1;
			ans=i;
			return ans;
		}
	}
	return ans;
}

// c). Periodically (using some timer), reset all the USE bits to 0.

void TLB_Insert(int VPN, int pfn, int valid,int present,int protect,int dirty,int supervisor,int tlb){
		int i=last;
		while(TLBE[i].active==1){
			i=(i+1)%tlb;
			if (i==last){
				for (int j=0;j<tlb;j++){
				TLBE[j].active=0;}
				break;}	
		}
		TLBE[i].active=1;
		TLBE[i].vpn=VPN;
		TLBE[i].pfn=pfn;
		TLBE[i].valid=valid;
		TLBE[i].present=present;
		TLBE[i].protect=protect;
		TLBE[i].dirty=dirty;
		TLBE[i].supervisor=supervisor;
		
}
int AccessMemory(int request, struct PageTableEntry *PTE,int tlb){
	int VPN = get_VPN(request);
	int offset = get_Offset(request);
	int success=TLB_Lookup(VPN,tlb);
	int physical_address;
	if (success!=-1){
		if (TLBE[success].protect>0){
			physical_address= (TLBE[success].pfn << 10)|offset;
			printf("\tPhysical Address:\t%s", dec2bin(physical_address));
			return physical_address;
		}
		else{
			printf("\tPROTECTION_FAULT");
			return -1;
		}
	}
	else{
		if (PTE[VPN].valid==0){
			printf("\tSEGMENTATION_FAULT");
			return -1;
		}
		else if(PTE[VPN].protect==0) {
			printf("\tPROTECTION_FAULT");
			return -1;
		}
		else{
			TLB_Insert(VPN, PTE[VPN].pfn, PTE[VPN].valid,PTE[VPN].present,PTE[VPN].protect,PTE[VPN].dirty,PTE[VPN].supervisor,tlb);
			AccessMemory(request,PTE,tlb);
		}
	}
	
	}


int initTLB(struct TLB_Entry *TLBE){
	// Make TLB's Cells Inactive
	for (int i = 0; i < TLB_SIZE; ++i){
		TLBE[i].active = 0;
		TLBE[i].vpn = -1;
	}
	return 1;
}

int main(int argc, char *argv[]){
	srand(time(0));
    xs=rand();
	struct PageTableEntry PTE[PAGES];
	initPageTable(PTE);
	initTLB(TLBE);
	int sequences[4]={25,50,75,100};
	int tlb_type[3]={8,16,32};
	int request; 
	int count = -1;
	srand(time(NULL));
	struct timeval time_1, time_2;

	for (int i=0;i<3;i++){
		printf("\n=================================FOR TLB_TYPE : %d=================================\n",tlb_type[i]);
		for (int j=0;j<4;j++){
			printf("\n=================================FOR SEQUENCE OF %d=================================\n",sequences[j]);
			double overall_time=0;
			int correct=0;
			for (int k=0;k<10;k++){
			for (int l=0;l<sequences[j];l++){
			struct timeval  start, end;
			double time;
				request=xorshift()%65536;
				printf("Virtual address:\t %s ",dec2bin(request));
				gettimeofday(&start, NULL);
				int success = AccessMemory(request, PTE,tlb_type[i]);
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

			
	}}
	//printf("Average time taken in this trial %f\n",overall_time/correct);
		printf("\n==============================================\n");
        printf("||Average time taken in this trial %f ||\n",overall_time/correct);
        printf("==============================================\n");
	}}

	return 0;
}