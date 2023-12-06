/*
  Fichier : shell_cp_executetocomplete.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>

/*---------------------------------------------*/

char *sh_read_line(FILE *f){
  char *line = NULL;
  size_t bufsize = 0; 
  getline(&line, &bufsize, f);
  return line;
}

/*---------------------------------------------*/

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\n"

char ** sh_split_line( char *line){
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;
  
  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
  
  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;
    
    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

/*---------------------------------------------*/

int newF(const char *token) {
    char *forbidden = getenv("FORBIDDEN");
    char forbiddenCopy[1024]; 

    if (forbidden != NULL) {
        snprintf(forbiddenCopy, sizeof(forbiddenCopy), "%s:%s", forbidden, token);
    } else {
        snprintf(forbiddenCopy, sizeof(forbiddenCopy), "%s", token);
    }

    setenv("FORBIDDEN", forbiddenCopy, 1);
    //return 0;
}


int rmf(const char *token) {
    char *forbidden = getenv("FORBIDDEN");
    char forbiddenCopy[1024]; 
    forbiddenCopy[0] = '\0';
    char *index = strtok(forbidden, ":");
    while (index != NULL) {
        if (strcmp(index, token) != 0) {
            strcat(forbiddenCopy, index);
            strcat(forbiddenCopy, ":");
        }
        index = strtok(NULL, ":");
    }

    if (strlen(forbiddenCopy) > 0) {
        forbiddenCopy[strlen(forbiddenCopy) - 1] = '\0'; // Retirer le dernier ":"
    }

    setenv("FORBIDDEN", forbiddenCopy, 1);
    //return 0;
}

int isForbidden(char **args) {
    char *forbidden = getenv("FORBIDDEN");

        if(forbidden != NULL){
            char forbiddenCopy[1024]; 
            strcpy(forbiddenCopy, forbidden); 

            char *token = strtok(forbiddenCopy, ":");
          
            while (token != NULL) {
              for(int i = 0; args[i] != NULL; i++){
                if (strstr(args[i], token) != NULL) {   
                  return 1;
                }
              }
              token = strtok(NULL, ":");
            }
        }
        return 0;
}

/*---------------------------------------------*/

int sh_execute(char **args){
  int verif = 0; 
  if(args[0] != NULL){
    if (strcmp(args[0], "exit") == 0) {
      exit(0);
    } 
    else if (strcmp(args[0], "newf") == 0) {
        if (args[1] != NULL) {
            newF(args[1]);
            printf("nv chaine : %s\n", args[1]);
            verif = 1;
        } else {
           printf("pas de chaine\n"); 
            verif = 1;
        }
    }
    else if (strcmp(args[0], "rmf") == 0) {
        if (args[1] != NULL) {
            rmf(args[1]);
           printf("chaine sup : %s\n", args[1]); 
            verif = 1; 
        } else {
            printf("pas de chaine\n");
            verif = 1;
        }

    }
  int status;
  pid_t pid;
  pid = fork();
    
    if((strcmp(args[0],"exit") != 0 || strcmp(args[0],"newf") != 0 || strcmp(args[0],"rmf") != 0)){
      if (pid < 0){ fprintf(stderr,"A fork error has occurred.\n"); exit(-1);}
      else {
        if (pid == 0) {/* We are in the child. */
          if(isForbidden(args) == 0 && verif == 0) {
            execvp(args[0], args);
            perror("Erreur d'exécution de la commande");
            exit(EXIT_FAILURE);
          }
          else{
            if(verif != 1){printf("Travaille au lieu de jouer !\n");}
            exit(EXIT_FAILURE);
          }
        }
        else {/* We are in the parent. */
          wait(&status); /* Wait for the child to terminate. */
        }
      }
    }
  }

  return 1;
}

/*---------------------------------------------*/
void ignore_signal(int sig) {  
}

void sh_loop(void){
  char *prompt = "l3miage shell > ";
  char *line;
  char **args;
  int status;
  signal(SIGINT,ignore_signal);

  do {  
    printf("%s",prompt);
    fflush(stdout);
    line = sh_read_line(stdin);
    args = sh_split_line(line); 
    status = sh_execute(args);
  } while(status);  
}

/*==============================================*/

int main(int argc, char * argv[]){

  sh_loop();
    
  return EXIT_SUCCESS;
}

