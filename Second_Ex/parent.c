#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>



bool signal_R1 = false;
bool signal_R2 = false;
bool signal_TERM = false;
bool signal_CHLD = false;
bool sent_from_parent = false;

//handler for SIGUSR1
void handle_SIGUSR1(int signal){
	//flip state
	signal_R1 = true;
}

//handler for SIGUSR2
void handle_SIGUSR2(int signal){
	//print state
	signal_R2 = true;
}

//handler for SIGTERM
void handle_SIGUSTERM(int signal){
	//termatizi
	signal_TERM = true;
	sent_from_parent = true;
}

//handler for SIGCHLD
void handle_SIGCHLD(int signal){
	if(sent_from_parent == false)
		signal_CHLD = true;
}


int main(int argc, char *argv[]) {
	
	if(argc != 2){
		perror("Wrong input\n");
		exit(1);
	}
	
	pid_t child_pid = 0;
	
	
	//sigaction
	struct  sigaction sig_R1, sig_R2, sig_TERM, sig_CHLD ;
	sig_R1.sa_handler = handle_SIGUSR1;
	sig_R2.sa_handler = handle_SIGUSR2;
	sig_TERM.sa_handler = handle_SIGUSTERM;
	sig_CHLD.sa_handler = handle_SIGCHLD;
	
	sigaction(SIGUSR1, &sig_R1, NULL);
	sigaction(SIGUSR2, &sig_R2, NULL);
	sigaction(SIGTERM, &sig_TERM, NULL);
	sigaction(SIGCHLD, &sig_CHLD, NULL);
	
	int N = strlen(argv[1]);
	int wstatus;
	int pid_array[N];
	
	//check if there are only 2 arguements
	
	
	//check if argv[1] contains only 't' and 'f'
	for(int i = 0; i < N ; i++){
		if(argv[1][i] != 't' && argv[1][i] != 'f'){
			perror("Wrong input\n");
			exit (1);
			}
	}
	
	pid_t child;
	
	//create N children and make checks
	for(int i = 0; i < N; i++){
		child = fork();
		if(child == 0){
			printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", getppid(), i, getpid(), argv[1][i]);
			char stat = argv[1][i];
			char *const child_status[] = {"./child", &stat, NULL};
			int status = execv("./child", child_status);
			exit(0);
		}
		if(child == -1){
			perror("Error creating child\n");
			exit(1);
		}
			
		if(child != 0){
			pid_array[i] = child;
		}	
	}

	

	do{
		//SIGUSR1
		if(signal_R1 == true){
			for(int i = 0; i < N; i++)
				kill(pid_array[i], SIGUSR1);
			signal_R1 = false;
		}
		
		//SIGUSR2
		if(signal_R2 == true){
			for(int i = 0; i < N; i++)
				kill(pid_array[i], SIGUSR2);
			signal_R2 = false;
		}
		
		//SIGCHLD
		if(signal_CHLD == true){
			//MAKE A KID
			for(int l = 0; l < N; l++){
				if(child_pid == pid_array[l]){
					printf("[PARENT/PID=%d] Child %d with PID=%d exited\n", getpid(), l, pid_array[l]);

					child = fork();
					if(child == 0){
						printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n", getppid(), l, getpid(), argv[1][l]);
						char stat = argv[1][l];
						char *const child_status[] = {"./child", &stat, NULL};
						int status = execv("./child", child_status);
						exit(0);
					}
					if(child == -1){
						perror("Error creating child\n");
						exit(1);
					}
					if(child != 0){
						pid_array[l] = child;
					}
					
					
				}
					
			}
			signal_CHLD = false;
		}
		
		//SIGTERM
		if(signal_TERM == true){
			for(int i = 0; i < N; i++){
				printf("[PARENT/PID=%d] Waiting for %d children to exit\n", getpid(), N-i);
				kill(pid_array[i], SIGTERM);
				printf("[PARENT/PID=%d] Child with PID=%d terminated successfully with exit status code %d!\n", getpid(), pid_array[i], wstatus); 
			}
			printf("[PARENT/PID=%d] All children exited, terminating as well\n", getpid());
			exit(0);
		}
		
		//check for SIGSTOP
		if((child_pid = waitpid(-1, &wstatus, WUNTRACED)) != 0){
			if (WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGSTOP) {
       			kill(child_pid, SIGCONT);
       			wstatus = 0;
    			}
    		}
    		
	}while(1);
	
	
	return 0;
}
