/* Fichier : SANS tube_ls.c
Le pere cree un processus fils pour sous traiter la commande
MAIS c'est le fils qui affiche dans la console. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

void eliminer_zombie(int sig); /* Handler du signal SIGCHLD */

int faire_ls(int pfd[2]){ 
    printf("running ls %d\n",getpid());
    char cmd[] = "ls";
    char options[] = "-l";
    close(STDOUT_FILENO);
    close(pfd[0]); /* close the unused read side */
    dup2(pfd[1], STDOUT_FILENO); /* connect the write side with stdout */
    close(pfd[1]); /* close the write side
    => can be used to stop reading by next cmd in pipe : EOF */
    execlp(cmd, cmd, options, NULL, NULL);
    printf("ls failed"); /* if execlp returns, it's an error */
    return 1;
}

int main(int argc, char *argv[]){
    int pfd[2]; /* creation du tube */
    if (pipe(pfd) == -1) perror("impossible de creer le tube") ;
    signal(SIGCHLD, eliminer_zombie);
    printf("pere %d\n",getpid());
    if (fork() == 0){ /* Le fils ls */
        faire_ls(pfd);
    }
    else { /* Le pere */
        close(pfd[1]);
        sleep(1); /* Le temps que le signal .. */
        char msg[1024];
        printf("dans : %d\n",getpid());
        int v = 1;
        while(v>0){
            v = read(pfd[0], msg, sizeof(msg));
        }
        write(STDOUT_FILENO, msg, strlen(msg)+1); // Affiche les données reçues sur la sortie standard du père
        close(pfd[0]);
        exit(0); 
    }
}

void eliminer_zombie(int sig){ /* Handler du signal SIGCHLD */
    int exit_cond;
    pid_t pid;
    printf("Attente de la terminaison du fils...\n");
    pid = wait(&exit_cond); /* Pid du fils */
    if (WIFEXITED(exit_cond))
        printf("%d : termine correctement : %d\n", pid, WEXITSTATUS (exit_cond));
    else
        printf("%d : mal termine : %d\n", pid,WTERMSIG(exit_cond));
}