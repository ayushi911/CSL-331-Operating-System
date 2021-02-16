#include <sys/types.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h> 

/* Read characters from the pipe and echo them to stdout.*/
void read_from_pipe(int file) {
  /* the variable file is of integer type and
  it points a memory location that can be written 
  into*/
  char sentence[1000] = "";
  int check_num = 0;
  char * token;
  FILE * fptr;
  fptr= fdopen(file-2, "r");
  if (fptr == NULL) {
    printf("Error!");
    exit(1);
  }
  fgets(sentence, 1000, fptr);
  token = strtok(sentence, " ");
  fclose(fptr);
  /* walk through other tokens */
  while (token != NULL) {
    check_num = check_num + 1;
    if (strcmp(token, "0") == 0) {
      printf("%d\n", check_num);
    }
    token = strtok(NULL, " ");
  }
}

/* Write some random text to the pipe. */

void write_to_pipe(int file) {
  /* the variable file is of integer type and
  it points a memory location that can be written 
  into*/
  char sentence[288] = "";
  char num[10000];
  int numbers[100];
  FILE * fptr;
  for (int i = 0; i < 100; i++) {
    numbers[i] = i + 1;
  }
  /* Intializes random number generator */
  srand(time(0));
  int k=0;
  while(k<10){
     numbers[rand() % 100] = 0;
     k+=1;
  }
   
  
  strcpy(sentence, "");
  for (int i = 0; i < 100; i++) {
    sprintf(num, "%d", numbers[i]);
    strcat(sentence, num);
    strcat(sentence, " ");
  }
  // opening file in writing mode
  fptr = fdopen(file-2, "w");
  // exiting program 
  if (fptr == NULL) {
    printf("Error!");
    exit(1);
  }
  fprintf(fptr, "%s", sentence);
  fclose(fptr);
   int len = strlen("1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100");
   printf("|%d|", len);
  printf("%s", "Random zeros are added in the nos.\n");
}

int main(void) {
  pid_t pid;
  int mypipe[2];
  mypipe[0] = open("task3.txt", O_RDONLY);
  mypipe[1] = open("task3.txt", O_WRONLY | O_CREAT | O_TRUNC);
  /* Create the pipe. */
  if (pipe (mypipe))
    {
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
    }

  /* Create the child process. */
  pid = fork ();
  if (pid == (pid_t) 0)
    {
      close (mypipe[0]);
      write_to_pipe (mypipe[1]);
      return EXIT_SUCCESS;
    }
  else if (pid < (pid_t) 0)
    {
      /* The fork failed. */
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
    }
  else
    {
      wait(NULL);
      close (mypipe[1]);
      read_from_pipe(mypipe[0]);
      return EXIT_SUCCESS;
    }
    char c;
    FILE *f=fopen("task3.txt","r");
    if (f == NULL) 
    { 
        printf("Cannot open file \n"); 
        exit(0); 
    } 
  
    // Read contents from file 
    c = fgetc(f); 
    while (c != EOF) 
    { 
        printf ("%c", c); 
        c = fgetc(f); 
    } 
  
    fclose(f); 
    return 0; 
}


