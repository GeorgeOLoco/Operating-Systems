#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

struct stat statbuf;

int length (int num){
	int digits = 0;
	while(num / 10 != 0){
		num /= 10;
		digits++;
	}
	return digits + 1;
}

int main(int argc, char *argv[]){



//check if I put only 1 filename

//No filename
	if(argc == 1){
		perror("Usage: ./a.out filename\n");
		exit(1);
	}	

//check if filename is --help
	if(strcmp(argv[1], "--help") == 0){
		perror("Usage: ./a.out filename\n");
		exit (0);
	}


//More than 1 filename
	if(argc > 2){
		perror("Usage: ./a.out filename\n");
		exit(1);
	}


//If filename exist
	if(stat(argv[1], &statbuf) == 0){
		printf("Error: %s already exists\n", argv[1]);
		exit(1);
	}



//MAIN

	int fd = open(argv[1], O_CREAT | O_APPEND | O_WRONLY, 0644);
	
	if(fd == -1){
		perror("open");
		return 1;
	}
	
	
	int status;
	pid_t father;
	int pid_check = getpid();
	
	father = fork();

	if (father < 0){
		printf("Error: Pid less than zero\n");
	
	}

	if (father == 0){
		//child'S CODE
		
		int pid = getpid();
		int ppid = getppid();
		
		if(pid_check != ppid){
			perror("PID ERROR\n");
			return 0;
		}		
		
		char message1[strlen("[CHILD] getpid() = ")];
		char ppid_str[length(ppid)] ;
		char pid_str[length(pid)];
		char str3[strlen(", getppid() = ")];

		
		strcpy(message1, "[CHILD] getpid() = ");
		strcpy(str3, ", getppid() = ");
		
		sprintf(ppid_str, "%d", ppid); //pid to string 
		sprintf(pid_str, "%d", pid);

		
		strncat(message1, pid_str, strlen(pid_str)); //append
		strncat(message1, str3, strlen(str3));
		strncat(message1, ppid_str, strlen(ppid_str));
		strcat(message1, "\n");
		
		if(write(fd, message1, strlen(message1)) < strlen(message1)){
			perror("write");
			return 1;
		}
		
		return 0;
	}

	else{
		//Father's1 code
		
		int pid = getpid();
		int ppid = getppid();
			
		
		char message1[strlen("[PARENT] getpid() = ")];
		char ppid_str[length(ppid)] ;
		char pid_str[length(pid)];
		char str3[strlen(", getppid() = ")];
		
		
		strcpy(message1, "[PARENT] getpid() = ");
		strcpy(str3, ", getppid() = ");
		
		sprintf(ppid_str, "%d", ppid); //pid to string 
		sprintf(pid_str, "%d", pid);
		
		
		strncat(message1, pid_str, strlen(pid_str)); //append
		strncat(message1, str3, strlen(str3));
		strncat(message1, ppid_str, strlen(ppid_str));
		strcat(message1, "\n");
		
		if(write(fd, message1, strlen(message1)) < strlen(message1)){
			perror("write");
			return 1;
		}
		
		wait(&status);
		return 0;
	}

	close(fd);
	return (0);
}
