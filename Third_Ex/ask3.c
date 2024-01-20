#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <ctype.h>
#include <sys/select.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

bool Its_number(char str[]) {
   for (int m = 0; m < strlen(str); m++)
      if (isdigit(str[m]) == false)
      return false;
   return true;
}


int main(int argc, char *argv[]){

    pid_t childpid;
    int pick=0;//sharing=0->round,sharing=1->random

//Checking input
    if( argc < 2 || argc > 3){
        perror("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
        exit(1);
    }else if(argc == 3){
        if(strcmp(argv[2],"--random") == 0){
            pick = 1;
        }else if(!(strcmp(argv[2],"--round-robin") == 0)){
            perror("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
            exit(1);
        }
    }
    int N = atoi(argv[1]);
    if(N < 1){
        perror("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
        exit(1);
    }


//Creating pipes

    int child[N];
    int CtoP[N][2];
    int PtoC[N][2];
    int i;

    for (i = 0; i < N; i++){

        pipe(&PtoC[i][0]);
        if (pipe(&PtoC[i][0])!= 0) {
           printf("error in pipe()\n");
            exit(1);
        }

        pipe(&CtoP[i][0]);
        if (pipe(&CtoP[i][0])!= 0){
            printf("error in pipe()\n");
            exit(1);
         }
    }

//childs

    for(i=0;i<N;i++){

        if((childpid = fork())<0){
            printf("error in fork()\n");
            exit(1);
        }else if(childpid == 0){
            int x;

            close(CtoP[i][0]);
            close(PtoC[i][1]);
            while(1){

                //int fd_input=read(PtoC[i][0], &x, sizeof(int));

                if(read(PtoC[i][0], &x, sizeof(int)) == -1){
                    printf("error in read()\n");
                    exit(1);
                }

                printf("[Child %d]: [%d] Child received %d\n", i, getpid(), x);
                x++;
                sleep(5);

                //int fd_output=write(CtoP[i][1], &x, sizeof(int));// pou ta grafw,apo pou
                if( write(CtoP[i][1], &x, sizeof(int)) == -1){
                    printf("error in write()\n");
                    exit(1);
                }

                printf("[Child %d]: [%d] Child Finished hard work, writing back %d\n", i, getpid(), x);

            }
            exit(0);
        }

        child[i] = childpid;
    }

    for(int j=0;j<N;j++){
        close(CtoP[j][1]);
        close(PtoC[j][0]);
    }

//parent
     int cnt=0;
     while(1){
        fd_set inset;
        int maxfd;

        FD_ZERO(&inset);
        FD_SET(0, &inset);

        for(int j=0; j<N; j++)
            FD_SET(CtoP[j][0], &inset);


        maxfd=0;

        for(int k=0; k<N; k++)
            maxfd = MAX(maxfd,CtoP[k][0]);

        maxfd++;


        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL);

        if(ready_fds <= 0)
            printf("select");

//Reading from terminal

         if(FD_ISSET(0, &inset)) {
            char buf[101];
            int input_size = read(0, buf, 100);
            if(input_size == -1){
                    printf("error in read()");
                    exit(1);
            }

            buf[input_size] = '\0';

            //Removing '\n'
            if(input_size > 0 && buf[input_size-1] == '\n') {
                buf[input_size-1] = '\0';
            }



            if(input_size == 5  && strncmp(buf, "exit", 4) == 0){ //input=="error"
                for (int l = 0; l < N; l++){
                    int w = waitpid(child[l], NULL, WNOHANG);
                    if (w==0){
                        printf("Father[PID:%d] is terminating child %d:[child_PID:%d]\n",getpid(),l,child[l]);
                        kill(child[l],SIGTERM);
                        wait(NULL);
                    }
                    //Closing pipes
                    close(CtoP[l][0]);
                    close(PtoC[l][1]);
                }
                    exit(0);
            }else if(input_size >= 1 && Its_number(buf)){ //input==number

                int x=atoi(buf);
                int c;

                if (pick==0){ //round
                    c=cnt%N;
                    cnt++;
                 }else if (pick == 1){ //random
                    c=rand()%N;
                 }

                 // int Wbytes = write(readpd[tuxaios][1],&x,sizeof(int));
                 if (write(PtoC[c][1], &x, sizeof(int)) == -1){
                     printf("error in write()");
                     exit(1);
                 }
                 printf("[Parent] Assigned %d to child %d\n", x,c);

            }

            else if (input_size >= 1){ // input != "error" && input != number
                printf("Type a number to send job to a child!\n");
            }
        }




//Reading the response from any child


        for(int k=0; k<N; k++){

            if (FD_ISSET(CtoP[k][0], &inset)){

                int y;
                //int Rbytes=read(CtoP[k][0], &y, sizeof(int));
                if (read(CtoP[k][0], &y, sizeof(int)) == -1) {
                    perror("error in read()");
                    exit(1);
                }

             printf("[Parent] Received result form child %d --> %d.\n", k, y);
            }
        }

     }
    return 0;
}
