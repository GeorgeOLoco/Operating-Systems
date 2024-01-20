#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>


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
	
	if(argc == 3){
		if(strcmp(argv[2], "--round-robin") == 0)
			mode = true;
		else if(strcmp(argv[2], "--random") == 0)
			mode = false;
		else{
			printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
			exit(0);
		}
	}
	
	else if(argc != 2){
		printf("Usage: ask3 <nChildren> [--random] [--round-robin]\n");
		exit(0);
	}
		
	
	
	int max_fds_read = STDIN_FILENO;
	
	int N = atoi(argv[1]);
	int pid_array[N];

	int pd[2*N][2];	
	
	pid_t child[N];
	
	fd_set readfds;
	
	
	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);

	//make the pipes
	for(int i = 0; i < 2*N; i++)
		pipe(pd[i]);
	
	//add pipes to the sets
	for(int i = 0; i < 2*N; i++){
		FD_SET(pd[i][0], &readfds);
	}
	
	//find the highest discriptors number
	for(int i = 0; i < 2*N; i++){
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
				//close(pd[i][1]);
				read(pd[i][0], &num, sizeof(int));
				info[1] = num;
				printf("[Child %d] [%d] Child received %d!\n", i, getpid(), info[1]);
				info[1]++; 
				sleep(5);
				//close(pd[N+i][0]);
				write(pd[N+i][1], &info, 2*sizeof(int)); 
				printf("[Child %d] [%d] Child Finished hard work, writing back %d\n", i, getpid(), info[1]);
			}
							
			exit(0);
		}
		
		//Parents code	
		if(child[i] > 0){
			pid_array[i] = child[i];
			
		}
		
		//Error case
		if(child[i] == -1){
			perror("Error creating child\n");
			exit(1);
		}
			
	}
	
	//parents code
	
	int num;
	int info[2];
	int s = 0;
	bool flag = true;
	if(mode == true){
		while(1){
			//empty the sets
			FD_ZERO(&readfds);
			FD_SET(STDIN_FILENO, &readfds);
	
			//add pipes to the sets
			for(int i = N; i < 2*N; i++){
				FD_SET(pd[i][0], &readfds);
			}
	
			//find the highest discriptors number
			for(int i = N; i < 2*N; i++){
				max_fds_read = max(max_fds_read, pd[i][0]);	
			}
			
			select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
			
			if (FD_ISSET(STDIN_FILENO, &readfds)) {
				char buf[101];
				flag = true;
				int n_read = read(STDIN_FILENO, buf, 100);
				buf[n_read] = '\0'; 											
				if (n_read > 0 && buf[n_read-1] == '\n') {
                			buf[n_read-1] = '\0';
                			n_read--;
            			}	
     				int f = 0;		
           			while(isdigit(buf[f]) && f < n_read){
            				f++;
            			}

            			if(f != n_read){
           				printf("Type a number\n");
            				
            			}
     				else{
     					num = atoi(buf);
     					//close(pd[s][0]);
					s =s % N;
					write(pd[s][1], &num, sizeof(int));
					
					s++;
           			}
            		}	
			
			for(int k = N; k < 2*N; k++){
				if(FD_ISSET(pd[k][0], &readfds)){
					//close(pd[k][1]);
					read(pd[k][0], &info, 2*sizeof(int));
					printf("Parent received from child %d number %d\n", info[0], info[1]);
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
