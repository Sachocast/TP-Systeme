#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 10

int main(int argc, char *argv[]) { 
    int looptime = 0; 
    socklen_t ls = sizeof(struct sockaddr_in);

    int sd_local;         
    int ps_local = 5001;   
    struct sockaddr_in adr_local, *padr_local = &adr_local;

    struct sockaddr_in adr_clients[MAX_CLIENTS];
    socklen_t ls_clients[MAX_CLIENTS];
    int num_clients = 0;

    char msg_in[10] = "0";    
    char msg_out[1000] = "0"; 

    if (argc != 1){
        fprintf(stderr,"Syntaxe d'appel : a.out  \n");
        exit(2);
    }

 
    if ((sd_local = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        perror("[SOCK_DGRAM, AF_INET, 0]");
    else
        printf("socket [SOCK_DGRAM, AF_INET, 0] creee\n");

    adr_local.sin_family      = AF_INET;
    adr_local.sin_addr.s_addr = htonl(INADDR_ANY);  
    adr_local.sin_port        = htons(ps_local);  

    if (bind(sd_local, (struct sockaddr *)(padr_local), ls) == -1) {
        perror("Attachement de la socket impossible");
        close(sd_local);  
        exit(2);       
    }

    getsockname(sd_local, (struct sockaddr *)padr_local, &ls);

    printf("\n... en attente de reception ...\n ");
    char x[1000] = "vide";
    for (;;) { 
        int i;
        struct sockaddr_in adr_client, *padr_client = &adr_client; 
        
        printf("\n------------------\n\n\nRequete reçue ");
        if (recvfrom(sd_local, msg_in, sizeof(msg_in), 0, (struct sockaddr *)&adr_client, &ls) == -1)  
            perror("Erreur de reception :");
        else  {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(adr_client.sin_addr), ip, INET_ADDRSTRLEN);

            int client_exists = 0;

            for (int j = 0; j < num_clients; ++j) {
                if (memcmp(&adr_client, &adr_clients[j], sizeof(struct sockaddr_in)) == 0) {
                    client_exists = 1;
                    break;
                }
            }

            if (!client_exists) {
                if (num_clients < MAX_CLIENTS) {
                    adr_clients[num_clients] = adr_client;
                    ls_clients[num_clients] = ls;
                    num_clients++;
                }
            }

            printf("%s --- de la machine cliente (%s) / port %d \n", msg_in, ip, (ntohs(adr_client.sin_port)));

            float num2; 
            char op;
            float res;
            char error[1000] = "bien";
            char num1;
            float xcalcule;
            sscanf(msg_in, "%c%c%f",&num1,&op,&num2);
            sscanf(x, "%f", &xcalcule);
                switch (op) {
                    case '+':
                        if (strcmp(x, "vide") == 0){
                            strcpy(error, "error");
                        }
                        res = xcalcule + num2;
                        break;
                    case '-':
                        if (strcmp(x, "vide") == 0){
                            strcpy(error, "error");
                        }
                        res = xcalcule - num2;
                        break;
                    case '*':
                        res = xcalcule * num2;
                        if (strcmp(x, "vide") == 0){
                            strcpy(error, "error");
                        }
                        break;
                    case '/':
                        if (strcmp(x, "vide") == 0){
                            strcpy(error, "error");
                        }
                        if (num2 != 0) {
                            res = xcalcule / num2;
                        } else {
                            strcpy(error, "division par 0 !");
                        }
                        break;
                    case '=':
                        sscanf(msg_in, "x=%s", x);
                        sscanf(x, "%f", &res);
                        break;
                    default:
                        break;
                }
            if(strcmp(error, "bien") != 0){
                snprintf(msg_out, sizeof(msg_out), "%s", error);
            }
            else
            {
                sprintf(msg_out, "%.f",res );
                strcpy(x, msg_out);
            }
        }


        printf("Reponse emise ");
        for (int j = 0; j < num_clients; ++j) {
            if (ntohs(adr_clients[j].sin_port) == (ntohs(adr_client.sin_port))) {
                if (sendto(sd_local, msg_out, sizeof(msg_out), 0, (struct sockaddr *)&adr_clients[j], ls_clients[j]) > 0) {
                    char ipEmission[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(adr_clients[j].sin_addr), ipEmission, INET_ADDRSTRLEN);
                    printf("%s --- à la machine cliente (%s) / port %d \n", msg_out, ipEmission, (ntohs(adr_clients[j].sin_port)));
                }
                else {
                    perror("Erreur d'envoi :");
                }
            }
        }

        sleep(1); looptime++;
    }

}
