
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
#include <time.h>

int main(int argc, char *argv[]){ 
  int looptime = 0; 
  socklen_t ls = sizeof(struct sockaddr_in); 

  int sd_local;          
  int ps_local = 0;   
  struct sockaddr_in adr_local, *padr_local = &adr_local; 

  struct sockaddr_in adr_dist,*padr_dist = &adr_dist;  
  struct hostent *hp_dist;      
    int ps_dist = 5001;

  char msg_in[10] = "0";  
  char msg_out[10] = "0";

  if (argc != 2){
    fprintf(stderr,"Syntaxe d'appel : a.out nom_du_host_peer \n");
    exit(2);
  }

  if ((sd_local=socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    perror("[SOCK_DGRAM, AF_INET, 0]");
  else
    printf("socket [SOCK_DGRAM, AF_INET, 0] creee\n");

  adr_local.sin_family      = AF_INET;
  adr_local.sin_addr.s_addr = htonl(INADDR_ANY);
  adr_local.sin_port        = htons(ps_local);  
  printf("Local Adress avant bind() :%s\n",inet_ntoa(adr_local.sin_addr));
  printf("Local Port avant bind() :%d\n",ntohs(adr_local.sin_port));
  if(bind(sd_local,(struct sockaddr *)(padr_local),ls) == -1) {
    perror("Attachement de la socket impossible");
    close(sd_local); 
    exit(2);      
  }
  getsockname(sd_local,(struct sockaddr *)padr_local,&ls);
  printf("Local Adress apres bind() :%s\n",inet_ntoa(adr_local.sin_addr));
  printf("Local Port apres bind() :%d\n",ntohs(adr_local.sin_port));


  hp_dist=gethostbyname(argv[1]);
  if(hp_dist == NULL){ 
    fprintf(stderr,"machine %s inconnue\n",argv[1]); 
    exit(2); 
  }
  else{ 
    memcpy(&adr_dist.sin_addr.s_addr, hp_dist->h_addr, hp_dist->h_length);
    adr_dist.sin_family = AF_INET;
    adr_dist.sin_port   = htons(ps_dist); 
    fprintf(stdout,"machine %s --> %s \n", hp_dist->h_name, inet_ntoa(adr_dist.sin_addr)); 
  }


  for(;;) { 
    int i;
    srand(time(NULL));
    char op[] = "+-*/=";
    int randOp = rand() %5;
    char variable = 'x';
    int rand2 = rand() % 10;
    sprintf(msg_out, "%c%c%d",variable, op[randOp],rand2);
    struct sockaddr_in adr2, *padr2 = &adr2; 
    printf("\n------------------\n\n\nRequete(%d) ... ", looptime);
    if (sendto(sd_local,msg_out, sizeof(msg_out), 0, (struct sockaddr *)padr_dist, ls) >0)
      printf("envoyee : valeur = %s !\n",msg_out);
    else
      printf("inacheve : %s !\n",msg_out);
    printf("Reponse(%d) ... ",looptime);
    if (recvfrom(sd_local, msg_in, sizeof(msg_in), 0, (struct sockaddr *)padr2, &ls) == -1)
      printf("inachevee : %s !\n",msg_in);
    else  {
      char ip[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &(adr2.sin_addr), ip, INET_ADDRSTRLEN);
      printf("recue : valeur = %s\n",msg_in);
      i = atoi(msg_in); 
      sprintf(msg_out,"%d",i);
    }
    sleep(1); looptime++;
  }
}
