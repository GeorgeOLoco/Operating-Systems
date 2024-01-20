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


int max(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}



int main(int argc, char *argv[]) {
	
	
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
	int max_fds_write = STDOUT_FILENO;	
	
//	int max_fds_read_p = STDIN_FILENO;
//	int max_fds_write_p = STDOUT_FILENO;
//	int max_fds_read_c = STDIN_FILENO;
//	int max_fds_write_c = STDOUT_FILENO;
	
	int N = atoi(argv[1]);
	int pid_array[N];

	int fd[N + 1][2];	
//	int fd_p[N][2];
//	int fd_c[N][2];
	
	pid_t child[N];
	
	fd_set readfds, writefds;
//	fd_set readfds_p, writefds_p, readfds_c, writefds_c;
	

	
		
//	FD_ZERO(&readfds_p);
//	FD_ZERO(&writefds_p);
//	FD_SET(STDIN_FILENO, &readfds_p);
//	FD_SET(STDOUT_FILENO, &writefds_p);
	
//	FD_ZERO(&readfds_c);
//	FD_ZERO(&writefds_c);
//	FD_SET(STDIN_FILENO, &readfds_c);
//	FD_SET(STDOUT_FILENO, &writefds_c);
		
	
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(STDOUT_FILENO, &writefds);

	
	for(int i = 0; i <= N; i++){
		pipe(fd[i]);
	}
	for(int i = 0; i <= N; i++){
		FD_SET(fd[i][0], &readfds);
		FD_SET(fd[i][1], &writefds);
	}
	
	
	//create N children and make checks
	for(int i = 0; i < N; i++){
		pipe(fd[i]);	//create i pipes
		//pipe(fd_c[i]);
		
		FD_SET(fd[i][0], &readfds);	//add pipes to set readfds and writefds
		FD_SET(fd[i][1], &writefds);		
		
//		FD_SET(fd_p[i][0], &readfds_p);	//add pipes to set readfds and writefds
//		FD_SET(fd_p[i][1], &writefds_p);
		
//		FD_SET(fd_c[i][0], &readfds_c);	//add pipes to set readfds and writefds
//		FD_SET(fd_c[i][1], &writefds_c);
		
		for(int i = 0; i <= N; i++){
			max_fds_read = max(max_fds_read, fd[i][0]);	//find the highest discriptors number
			max_fds_write = max(max_fds_write, fd[i][1]);
		}
	
		child[i] = fork();	//FORK
		
		//childs code
		if(child[i] == 0){			
			int timi;
			int info[2];
			info[0] = i;
			while(1){
				//select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
				close(fd[i][1]);
				read(fd[i][0], &timi, sizeof(int));
				info[1] = timi;
				printf("[Child %d] [%d] Child received %d!\n", i, getpid(), timi);
				info[1]++; 
				sleep(5);
				select(max_fds_write + 1, NULL, &writefds, NULL, NULL);
				close(fd[N][0]);
				write(fd[N][1], &info, 2*sizeof(int)); 
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

	
	char buf[101];
	int num;
	int info[2];
	bool flag = true;
	if(mode == true){
	
		while(1){
			FD_ZERO(&readfds);
			FD_ZERO(&writefds);
			FD_SET(fd[N][0], &readfds);
			FD_SET(fd[N][1], &writefds);
			for(int i = 0; i < N; i++){
				
				
				select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
				
				 printf("ISSET VALUE %d\n", FD_ISSET(fd[N][0], &readfds));
				do{
					flag = true;
					if (FD_ISSET(STDIN_FILENO, &readfds)) {
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
            						flag = false;
            					}
            				}
				
				}while(!flag);

				num = atoi(buf);
				
				select(max_fds_write + 1, NULL, &writefds, NULL, NULL);
				close(fd[i][0]);
				write(fd[i][1], &num, sizeof(int));
				//select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
				
				if (FD_ISSET(fd[N][0], &readfds)) {
					close(fd[N][1]);
					read(fd[N][0], &info, 2*sizeof(int));
					printf("Parent received from chiled %d number %d\n", info[0], info[1]);
				}
			}
		}
	}
	
	else if(mode == false){
		int i = rand()%N;
			select(max_fds_write + 1, NULL, &writefds, NULL, NULL);
			close(fd[i][0]);
			write(fd[i][1], &num, sizeof(int));
			select(max_fds_read + 1, &readfds, NULL, NULL, NULL);
			read(fd[i][0], &num, sizeof(int));
			printf("O pateras tipose : %d ... %d\n", i, num);
	}
	
	
	int status;
	for(int i = 0; i < N; i++){
		wait(&status);
	}
	
	

	return 0;
}
