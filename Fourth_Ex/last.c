#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/select.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#define BUFSIZE 1024 

//function to find the max between 2 ints
int max(int x, int y){
    if(x > y)
        return x;
    else
        return y;
}

//function to check if there are scaces in a string
bool check_for_space(char* arr){
	for(int i = 0; i < sizeof(arr); i++){
		if(arr[i] == ' ')
			return false;
	}
	return true;
}

//function to check if the string is get
bool is_get(char* arr){
	for(int i = 0; i < sizeof(arr); i++){
		if(arr[i] == ' ' || isdigit(arr[i])){}
		else
			return false;
	}
	return true;	
}

int main(int argc, char *argv[]){

	
	char host[100];
	char port_s[100];
	int port;
	bool debug = false;
	
	strcpy(host,"iot.dslab.pub.ds.open-cloud.xyz");
	port = 18080;
	
	if(argc == 1){
		strcpy(host,"iot.dslab.pub.ds.open-cloud.xyz");
		port = 18080;
	}
	
	else if(argc == 2){
		//check the arguments
		if(strncmp(argv[1], "--debug", 7) == 0 && strlen(argv[1]) == 7){
			debug = true;
		}
		else{
			perror("Wrong input\n");
			exit(-1);
		}
	}
	
	else if(argc == 3){
		//check the arguments
		char string[100];
		for(int i = 1; i < 3; i++){
			if(strncmp(argv[i], "--debug", 7) == 0){
				perror("Wrong input\n");
				exit(-1);		
			}
		}
		strcpy(string, argv[1]);
		if(strncmp(argv[1], "--host", 6) == 0){
			strcpy(host, "");
			strcpy(host, argv[2]);
		}
		else if(strncmp(argv[1], "--port", 6) == 0){
			strcpy(port_s, "");
			strcpy(port_s, argv[2]);
			port = atoi(port_s);
		}
		else{
			perror("Wrong input\n");
			exit(-1);
		}
	}
		
	else if(argc == 4){
		//check the arguments
		char string[100];
		int place;
		for(int i = 1; i < 4; i+=2){
			if(strncmp(argv[i], "--debug", 7) == 0){
				debug = true;
				place = i;		
			}
		}
		
		if(debug == false ){
			perror("Wrong input\n");
			exit(-1);
		}
		if(place == 1){
			strcpy(string, argv[2]);
			if(strncmp(argv[2], "--host", 6) == 0){
				strcpy(host, "");
				strcpy(host, argv[3]);
			}
			else if(strncmp(argv[2], "--port", 6) == 0){
				strcpy(port_s, "");
				strcpy(port_s, argv[3]);
				port = atoi(port_s);
				printf("PORT %d\n", port);
			}
			else{
				perror("Wrong input\n");
				exit(-1);
			}
		}
		
		else if(place == 3){
			strcpy(string, argv[1]);
			if(strncmp(argv[1], "--host", 6) == 0){
				strcpy(host, "");
				strcpy(host, argv[2]);
			}
			else if(strncmp(argv[1], "--port", 6) == 0){
				strcpy(port_s, "");
				strcpy(port_s, argv[2]);
				port = atoi(port_s);
				printf("PORT %d\n", port);
			}
			else{
				perror("Wrong input\n");
				exit(-1);
			}
		}
		
		else{
			perror("Wrong input\n");
			exit(-1);
		}
	}
	
	else if(argc == 5){
		//check the arguments
		char string[100];
		int place;
		for(int i = 1; i < 5; i++){
			if(strncmp(argv[i], "--debug", 7) == 0){
				perror("Wrong input\n");
				exit(-1);		
			}
		}

			strcpy(string, argv[1]);
			if(strncmp(argv[1], "--host", 6) == 0 && (strncmp(argv[3], "--port", 6) == 0)){
				
				strcpy(host, "");
				strcpy(host, argv[2]);
				
				strcpy(port_s, "");
				strcpy(port_s, argv[4]);
				port = atoi(port_s);
			}
			else if(strncmp(argv[1], "--port", 6) == 0 && (strncmp(argv[3], "--host", 6) == 0)){
				strcpy(port_s, "");
				strcpy(port_s, argv[2]);
				port = atoi(port_s);
				
				strcpy(host, "");
				strcpy(host, argv[4]);
			}
		
			else{
				perror("Wrong input\n");
				exit(-1);
			}
	}
	
	else if(argc == 6){
		//check the arguments
		char string[100];
		int place;
		for(int i = 1; i < 6; i+=2){
			if(strncmp(argv[i], "--debug", 7) == 0){
				debug = true;
				place = i;		
			}
		}
		
		if(debug == false ){
			perror("Wrong input\n");
			exit(-1);
		}
		
		if(place == 1){
			strcpy(string, argv[2]);
			if(strncmp(argv[2], "--host", 6) == 0 && (strncmp(argv[4], "--port", 6) == 0)){
				
				strcpy(host, "");
				strcpy(host, argv[3]);
				
				strcpy(port_s, "");
				strcpy(port_s, argv[5]);
				port = atoi(port_s);
			}
			else if(strncmp(argv[2], "--port", 6) == 0 && (strncmp(argv[4], "--host", 6) == 0)){
				strcpy(port_s, "");
				strcpy(port_s, argv[3]);
				port = atoi(port_s);
				
				strcpy(host, "");
				strcpy(host, argv[5]);
			}
		
			else{
				perror("Wrong input\n");
				exit(-1);
			}
		}
		
		else if(place == 3){
			strcpy(string, argv[1]);
			if(strncmp(argv[1], "--host", 6) == 0 && (strncmp(argv[4], "--port", 6) == 0)){
				strcpy(host, "");
				strcpy(host, argv[2]);
				
				strcpy(port_s, "");
				strcpy(port_s, argv[5]);
				port = atoi(port_s);
				
			}
			else if(strncmp(argv[1], "--port", 6) == 0 && (strncmp(argv[4], "--host", 6) == 0)){
				strcpy(port_s, "");
				strcpy(port_s, argv[2]);
				port = atoi(port_s);
				
				strcpy(host, "");
				strcpy(host, argv[5]);
			}
			else{
				perror("Wrong input\n");
				exit(-1);
			}
		}
		
	
	}
	
	else{
		perror("Wrong input\n");
		exit(-1);
	}
	
	
	struct sockaddr_in sin;
	struct hostent *server;
	//returns the structure of the server (hostent struct)
	server = gethostbyname(host);
	
	//create the socket and check it
	int sd;
	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0){
		perror("Error with socket\n");
		exit(-1);
	}
	
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port); /* Let the system choose */
	
	bcopy((char *) server->h_addr, (char *) &sin.sin_addr.s_addr, sizeof(server->h_length));
	
	//connect the socket referred to by the file descriptor sd to the address specified by addr
	int status = connect(sd,(struct sockaddr *) &sin, sizeof(sin));
	if(status < 0){
		perror("Error with connect\n");
		exit(-1);
	}
	
	//printf("CONECTED TO ADDRESS : %s\n", inet_ntoa( sin.sin_addr ));
	
	fd_set readsds;
	int max_sd_read = max(STDIN_FILENO, sd) + 1;
	char buf[101];
	
	while(1){
		FD_ZERO(&readsds);
		FD_SET(STDIN_FILENO, &readsds);
		FD_SET(sd, &readsds);
		max_sd_read = max(STDIN_FILENO, sd) + 1;
		int selectoras = select(max_sd_read, &readsds, NULL, NULL, NULL);
		
		if(selectoras == -1){
			perror("Error with select\n");
			exit(4);
		}
		
		//an estile kati o server
		char response[1024];
		if(FD_ISSET(sd, &readsds)){
			int n_read = read(sd, &response, sizeof(response));
			if(n_read == -1){
				perror("Error with read\n");
				exit(2);
			}
			
			response[n_read] = '\0';
			while(response[n_read-1] == '\n' && n_read > 0){
				response[n_read-1] = '\0';
                		n_read--;
            		}
            		
            		if(debug == true){
				printf("[Debug] read '%s'\n", response);
				printf("-------------------------------------------------------\n");
			}
            		
            		if(check_for_space(response) == true)
            			printf("Send verification code: '%s'\n", response);
			
			else if(strncmp(response, "try again", sizeof("try again")) == 0 || strncmp(response, "invalid code", sizeof("invalid code")) == 0)
				printf("%s\n", response);
			
			else if(is_get(response) == true){
				
				char g[21];
				
				strncpy(g, response, 21);
				
				switch(g[0]){
					case '0':
					printf("boot(%c)\n", g[0]);
					break;
					case '1':
					printf("setup(%c)\n", g[0]);
					break;
					case '2':
					printf("interval(%c)\n", g[0]);
					break;
					case '3':
					printf("button(%c)\n", g[0]);
					break;
					case '4':
					printf("motion(%c)\n", g[0]);
					break;
				}
				
				char temp[4];
				strncpy(temp, g+6, 5);
				double temperature = atoi(temp)/100.00;
				printf("Tempetature is: %.2f\n", temperature);
				
				if(g[2] == '0')
					printf("Light level is: %c%c\n", g[3], g[4]);
				else
					printf("Light level is: %c%c%c\n", g[2], g[3], g[4]);
				
				char time[10];
				strncpy(time,g+11,11);
				time_t rawtimen = atoi(time); 
            			struct tm *info;
          
         	 		info = localtime( &rawtimen );
         	 		int year = 1900 + info->tm_year;
         	 		int month = info->tm_mon;
         	 		int hour = info->tm_hour;
         	 		int min = info->tm_min;
         	 		int sec = info->tm_sec;
         	 		int day = info->tm_mday;
         	 	
          	 	 	printf("Timestamp is: %d-%d-%d %d:%d:%d\n", year, month, day, hour, min, sec);	
          	 	 	
			}
			else{
				char ack[n_read];
				strncpy(ack, response, n_read);
				ack[n_read] = '\0';
				if(ack[n_read - 1] == '\n')
					ack[n_read - 1] = '\0';

				printf("Response: '%s'\n", ack);
			}
			
		}

		
		//check if we can read from terminal and check for errors
		if (FD_ISSET(STDIN_FILENO, &readsds)) {
			int n_read = read(STDIN_FILENO, &buf, 100);
			
			//error check for read
			if(n_read == -1){
				perror("Error with read\n");
				exit(2);
			}
			//check if input is exit
			if(n_read == 5 && strncmp(buf, "exit", 4) == 0){
				printf("Terminating the program\n");
				close(sd);
				exit(0);
			}
			//check if input is help
			else if(n_read == 5 && strncmp(buf, "help", 4) == 0){
				printf("-------------------------------------------------------\n");
				printf("Type <help> for this message\n");
				printf("Type <exit> to terminate the program\n");
				printf("Type <get> for weather info\n");
				printf("Type <N name surname reason> to make a transfer request\n");
				printf("-------------------------------------------------------\n");
			}
			
			else{
				char msg[n_read];
				strncpy(msg, buf, n_read);
				int n_write = write(sd, &msg, sizeof(msg));
				if(n_write == -1){
					perror("Error with write\n");
					exit(2);
				}
				buf[n_read - 1] = '\0'; 
				if(debug == true){
					printf("-------------------------------------------------------\n");
					printf("[DEBUG] sent '%s'\n", buf);
				}
			}
		}
	}
	
	
	
	

	close(sd);
	return 0;
}

