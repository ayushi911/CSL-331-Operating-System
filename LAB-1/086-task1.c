#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include <unistd.h>
#include<sys/wait.h>

int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int main(char *argv[], int argc){
    int i,s;
    srand(time(0));
    FILE *f = fopen("temp.bin","wb");
    i=1;
    while(i<=100){
        s = fwrite(&i, sizeof(i), 1, f);
        ++i;
    }
    fclose(f);
    //forking a child process
    pid_t pid;
    pid = fork();
    //for the parent process
    if(pid!=0){
        wait(NULL);
        int arr[10];
        memset(arr,-1,10*sizeof(arr[0]));
        int j=0;
        FILE *f= fopen("temp.bin", "rb");
        int read_val;

        for(int i=0;i<100;i++){
            fread(&read_val,sizeof(read_val),1,f);
            if(read_val==0){
                arr[j]=i;
                j++;
                }
            }
        int indices[10];
        printf("indices: ");
        for(int i=0;i<10;++i)
            if(arr[i]!= -1)
                indices[i]= arr[i];
        qsort(indices, 10, sizeof(int), cmpfunc);
        for(int i=0;i<10;++i)
            printf("%d ", indices[i]);
        printf("\n");
        fclose(f);
    }
    //child process
    else{
        int random_zero[10];
        printf("random nos chosen to make zero: ");
        for(int i=0;i<10;i++){
            int random=rand()%100;
            random_zero[i]=random;
            printf("%d ", random);
        }
        printf("\n");
        FILE *f = fopen("temp.bin", "rb+");
        for(int i=0;i<10;++i){
            int v=random_zero[i];
            fseek(f,v*sizeof(int),SEEK_SET);
            int setval=0;
            fwrite(&setval, sizeof(setval),1,f);
        }
        fclose(f);
        exit(0);
    }
    
    return 0;
}
