#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <stdbool.h>
#include <ctype.h>


#define Red "\033[1;31m"
#define Green "\033[1;32m"
#define Blue "\033[1;34m"
#define Yellow "\033[1;33m"
#define Purple "\033[1;35m"
#define Cyan "\033[1;36m"
#define White "\033[1;37m"

//function to find the max between 2 ints
int max(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}



int main(int argc, char *argv[]) {
	
	//mode true for round robin and false for random
	bool mode = true;
	
	//argument checks
	if(argc == 3){
		if(strcmp(argv[2], "--round-robin") == 0)
			mode = true;
		else if(strcmp(argv[2], "--random") == 0)
			mode = false;
		else{
			printf(White"Usage: ask3 <nChildren> [--random] [--round-robin]"White"\n");
			exit(0);
		}
	}
	
	else if(argc != 2){
		printf(White"Usage: ask3 <nChildren> [--random] [--round-robin]"White"\n");
		exit(0);
	}
		
	
	
	
	//check if N is an integer
	for (int i = 0; i < strlen(argv[1]); i++){ 
		if (isdigit(argv[1][i]) == false){
      			printf(White"Usage: ask3 <nChildren> [--random] [--round-robin]"White"\n");
			exit(0);
		}
	}
	
	int N = atoi(argv[1]);
	//check if N > 0
	if(N <= 0){
		printf(White"Usage: ask3 <nChildren> [--random] [--round-robin]"White"\n");
		exit(0);
	}
	int max_fds_read = STDIN_FILENO;
	int pid_array[N];

	int pd[N + 1][2];	
	
	pid_t child[N];
	
	fd_set readfds;
	
	
	FD_ZERO(&readfds); //remove all file descriptors from readfds
	FD_SET(STDIN_FILENO, &readfds); //add STDIN_FILENO to readfds

	//make the pipes and check
	for(int i = 0; i <= N; i++){
		if(pipe(pd[i]) != 0){
			perror(Red"Error with pipe"White"\n");
			exit(1);		
		}
	}
	
	//add file discriptors to readfds
	for(int i = 0; i <= N; i++){
		FD_SET(pd[i][0], &readfds); 
	}
	
	//find the highest discriptors number
	for(int i = 0; i <= N; i++){
		max_fds_read = max(max_fds_read, pd[i][0]);	
	}
	
	//create N children and make checks
	for(int i = 0; i < N; i++){
		
		child[i] = fork();	//FORK
		
		//childs code
		if(child[i] == 0){			
			int num;
			int info[2];
			info[0] = i;
			while(1){
				
				if(read(pd[i][0], &num, sizeof(int)) == -1){
					perror(Red"Error with read"White"\n");
					exit(2);	
				}
				
				info[1] = num;
				printf(Yellow"[Child %d] [%d] Child received %d!"White"\n", i, getpid(), info[1]);
				info[1]++; 
				sleep(5);
				
				if(write(pd[N][1], &info, 2*sizeof(int)) == -1){
					perror(Red"Error with write"White"\n");
					exit(3);
				}
				 
				printf(Green"[Child %d] [%d] Child Finished hard work, writing back %d"White"\n", i, getpid(), info[1]);
			}
							
			exit(0);
		}
		
		//Parents code	
		if(child[i] > 0){
			pid_array[i] = child[i];
			
		}
		
		//Error case
		if(child[i] == -1){
			perror(Red"Error creating child"White"\n");
			exit(1);
		}
			
	}
	
	//parents code
	char buf[101];
	int num;
	int info[2];
	int i = 0;
	
	while(1){
		
		FD_ZERO(&readfds); //removes all file descriptors from readfds
		FD_SET(STDIN_FILENO, &readfds); //add STDIN_FILENO to readfds

		//add file discriptors to readfds
		for(int i = 0; i <= N; i++){
			FD_SET(pd[i][0], &readfds);
		}

		//find the highest discriptors number
		for(int i = 0; i <= N; i++){
			max_fds_read = max(max_fds_read, pd[i][0]);	
		}
		
		int selectoras = select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
		
		//error check for select
		if(selectoras == -1){
			perror(Red"Error with select"White"\n");
			exit(4);
		}
		
		//check if pd[N][0] is ready to read and check for errors
		if(FD_ISSET(pd[N][0], &readfds)){
			if(read(pd[N][0], &info, 2*sizeof(int)) == -1){
				perror(Red"Error with read"White"\n");
				exit(2);
			}
			printf(Cyan"[Parent] received result from child %d --> %d"White"\n", info[0], info[1]);
		}
		
		//check if we can read from terminal and check for errors
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			int n_read = read(STDIN_FILENO, buf, 100);
			
			//error check for read
			if(n_read == -1){
				perror(Red"Error with read"White"\n");
				exit(2);
			}
			//check if input is exit and terminate children
			if(n_read == 5 && strncmp(buf, "exit", 4) == 0){
				for(int x = 0; x <= N; x++){
					close(pd[x][0]);
					close(pd[x][1]);
				}
				
				for(int l = 0; l < N; l++){
					kill(pid_array[l], SIGTERM);
					printf(Red"Child %d terminated!"White"\n", pid_array[l]);
				}
				printf(Red"All children are dead!"White"\n");
				return 0;
			}
			
			//check if input is number
			buf[n_read] = '\0'; 											
			if (n_read > 0 && buf[n_read-1] == '\n') {
               		buf[n_read-1] = '\0';
               		n_read--;
            		}	
     			int f = 0;						
          		while(isdigit(buf[f]) && f < n_read){
            			f++;
            		}
            		
            		if(f != n_read || f == 0){
           			printf(Blue"Type a number to assign job to a child!"White"\n");
            			
            		}
     			else{
     				//if round robin
     				if(mode == true){
     					num = atoi(buf);
					i =i % N;
					if(write(pd[i][1], &num, sizeof(int)) == -1){
						perror(Red"Error with write"White"\n");
						exit(3);
					
					}
					printf(Purple"[Parent] assingned %d to child %d"White"\n", num, i);
					i++;
				}
				//if random
				else if(mode == false){
					num = atoi(buf);
					int d = rand()%N;
					if(write(pd[d][1], &num, sizeof(int)) == -1){
						perror(Red"Error with write"White"\n");
						exit(3);
					}
					printf(Purple"[Parent] assingned %d to child %d"White"\n", num, d);
				}
           		}
            	}	

			
			
						
	}
	
	int status;
	//wait for N children
	for(int i = 0; i < N; i++)
		wait(&status);
		
	return 0;
}
