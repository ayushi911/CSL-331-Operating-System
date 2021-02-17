#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<sys/wait.h>
#include <ctype.h> 
#include<time.h>

// A linked list node 
struct Node { 
    char data[1024]; 
    struct Node* next; 
};

//-----------------------------------------PARSE----------------------------
void parse(char *line, char **args){
    if (strcmp(line, "exit\n") == 0)
            exit(EXIT_SUCCESS);
        char **next = args;
        char *temp = strtok(line, " \n");
        while (temp != NULL)
        {
            *next++ = temp;
            temp = strtok(NULL, " \n");
        }
        *next = NULL;
        for (next = args; *next != 0; next++)
            puts(*next);
}
//----------------------------------------APPEND-------------------------------
void append(struct Node** head, char *new_data) 
{
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
	struct Node *last = *head;
  strcpy(new_node-> data , new_data);  
	new_node->next = NULL; 
	if (*head == NULL) 
	{ 
	*head = new_node; 
	return; 
	} 
	while (last->next != NULL) 
		last = last->next; 
	last->next = new_node; 
	return;	 
}

//--------------------------------------PRINT-----------------------------------
void printdata(struct Node* n, char typegiven[24] ) 
{ 
  if (strncmp(typegiven,"BRIEF",strlen("BRIEF"))==0){
    int i=1;
    char temp[1024];
    char* token;
    while (n !=NULL) {
      strncpy(temp,n->data,strlen(n->data));
      token= strtok(temp," ");
      printf("%d ",i); 
        printf(" %s \n",token); 
        n = n->next; 
        ++i;
    }
  } 
   else if(strncmp(typegiven,"FULL",strlen("FULL"))==0){
     int i=1;
     while (n != NULL) {
       printf("%d ",i); 
        printf(" %s \n", n->data);
        ++i; 
        n = n->next; 
    } 
   }
   else{
     printf("WRONG COMMAND!!"); 
     return;
   }
}
//--------------------------------------CHECKNUMLIST--------------------------------------
void checknumList(struct Node* n, char *line){
    int f= atoi(line);
    int i=1;  
    while(n!=NULL){
      if(f!=i){
        ++i;
        n=n->next;
      } 
    }
    if(f==i){
       pid_t child_pid;
        if ((child_pid = fork()) == 0){
		                    char *ptr[64];
                        char temp[1024];
                        strncpy(temp,n->data,strlen(n->data));
		                    parse(temp,ptr);
                        if (execvp(ptr[0], ptr) < 0) {     /* execute the command  */
                            printf("* ERROR: exec failed *\n");
                            exit(1);
                            }
		                exit(EXIT_SUCCESS);
		            }
    }
    else
      printf("Numbered command not Present!!\n");

}
//--------------------------------------CHECKLIST--------------------------------------------
void checkList(struct Node* n, char *line){
        pid_t child_pid;
        if ((child_pid = fork()) == 0){
		                    char *ptr[64];
		                    parse(line,ptr);
                        if (execvp(ptr[0], ptr) < 0) {     /* execute the command  */
                            printf("* ERROR: exec failed *\n");
                            exit(1);
                            }
		                exit(EXIT_SUCCESS);
		            }
      append(&n,line);
    }
//-------------------------------------FREELIST------------------------------------------------

void freeList(struct Node** head_ref)
{
   struct Node* current = *head_ref;
   struct Node* next;
 
   while (current != NULL) 
   {
       next = current->next;
       free(current);
       current = next;
   }
   *head_ref = NULL;
}

//--------------------------------------MAIN----------------------------------------------------
int main( int argc, char *argv[] )  
{  struct Node* head = NULL;              
   pid_t child_pid, wpid;
   int status = 0;     
   if (argc==1){
	printf("No file names are mentioned \n");}
   
   else{
        for (int i=1;i<argc;i++){
	        FILE* filePointer;
	        char line[1024];
	        filePointer = fopen(argv[i], "r");
	        while(EOF != fscanf(filePointer, "%30[^\n]\n", line)) {
                    if ((child_pid = fork()) == 0){
		                    char *ptr[64];
		                    parse(line,ptr);
                        if (execvp(ptr[0], ptr) < 0) {     /* execute the command  */
                            printf("* ERROR: exec failed *\n");
                            exit(1);
                            }
		                exit(EXIT_SUCCESS);
		            }
                    else{
                        wait(NULL);
                        append(&head,line);
                        }
	            }
	        fclose(filePointer);
        }
  while ((wpid = wait(&status)) > 0);
      while(1){
      printf("Please enter your command: ");
      char  line[1024];             
      fgets(line, 1024, stdin);
      if (strncmp(line,"HISTORY BRIEF",strlen("HISTORY BRIEF"))==0){
        printdata(head,"BRIEF");
      }
      else if (strncmp(line,"HISTORY FULL",strlen("HISTORY FULL"))==0){
        printdata(head,"FULL");

      }
      else if (strncmp(line,"EXEC",strlen("EXEC"))==0){
        if (isdigit(line[5])){
        strncpy( line, line + 5,1000);
        checknumList(head,line);
        }
        else{
        strncpy( line, line + 5,1000);
        checkList(head,line);
        }
        
      }
      else if (strncmp(line,"STOP",strlen("STOP"))==0){
        printf("Exiting normally, bye\n");
        freeList(&head);
        exit(0);
      }
      else{
      printf("The entered command is invalid\n");
    }
    }
    }
   return 0; 
}
