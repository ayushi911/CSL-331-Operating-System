#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>
int main() {
 char sentence[1000]="";
    char num[10000];
    int numbers[100];
 FILE *fptr;
 for(int i=0;i<100;i++){      
		numbers[i]=i+1;    
}
    
   /* Intializes random number generator */
   srand(time(0));
   /* Print 5 random numbers from 0 to 49 */
   for( int i = 0 ; i < 10 ; i++ ) {
      numbers[rand() % 100]=0;
   }
 
  strcpy(sentence, "");
   for(int i=0;i<100;i++){      
		sprintf(num, "%d", numbers[i]);
    		strcat(sentence, num);
    		strcat(sentence, " ");    
}
// opening file in writing mode
    fptr = fopen("task2.txt", "w");

    // exiting program 
    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }
    fprintf(fptr, "%s", sentence);
    fclose(fptr);
  
        printf("%s","Random zeros are added by the child in the selected positions\n"); 
	exit(0);
    



return 0;
}
