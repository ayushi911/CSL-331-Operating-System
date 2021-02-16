#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>

int main() {
  char sentence[1000] = "", num[10000];
  int numbers[100];
  int check_num = 0;
  pid_t pid;
  char * token;

  // opening file in writing mode
  FILE *f = fopen("task2.txt", "w");

  // exiting program 
  if (f == NULL) {
    printf("Error!");
    exit(1);
  }
  for (int i = 1; i <= 100; ++i) {
    sprintf(num, "%d", i);
    strcat(sentence, num);
    strcat(sentence, " ");
  }

  fprintf(f, "%s", sentence);
  
  fclose(f);
  pid = fork();
  if (pid != 0) { //PARENT PROCESS
     wait(NULL);
    FILE *f = fopen("task2.txt", "r");
    if (f == NULL) {
      printf("Error!");
      exit(1);
    }
    fgets(sentence, 1000, f);
    token = strtok(sentence, " ");
    fclose(f);
    printf("\nParent gets the zeros and identified them\n");
    /* walk through other tokens */
    while (token != NULL) {
      check_num = check_num + 1;
      if (strcmp(token, "0") == 0) {
        printf("%d\n", check_num);
      }
      token = strtok(NULL, " ");
    }
  } 
  else { //CHILD PROCESS
    char * args[] = {"./child",NULL};
    execv(args[0], args);
    exit(0);
  }

  return 0;
}
