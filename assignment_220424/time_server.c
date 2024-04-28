#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/wait.h>

int check_format(char buf[]) {
    if (strstr(buf,"GET_TIME dd/mm/yyyy") != NULL)
        return 1; 
    if (strstr(buf,"GET_TIME dd/mm/yy") != NULL)
        return 1; 
    if (strstr(buf,"GET_TIME mm/dd/yyyy") != NULL)
        return 1;
    if (strstr(buf,"GET_TIME mm/dd/yy") != NULL) 
    {
        return 1;
    }
    return 0;
}

void data_process(char buf[]) {
    time_t cur_time = time(NULL);
    if (strstr(buf,"dd/mm/yyyy") != NULL)
    {
        strftime(buf, 80, "%d-%m-%Y", localtime(&cur_time));
    }
    else if (strstr(buf,"dd/mm/yy") != NULL)
    {
        strftime(buf, 80, "%d-%m-%y", localtime(&cur_time));
    }
    else if (strstr(buf,"mm/dd/yyyy") != NULL)
    {
        strftime(buf, 80, "%m-%d-%Y", localtime(&cur_time));
    }
    else if (strstr(buf,"mm/dd/yy") != NULL)
    {
        strftime(buf, 80, "%m-%d-%y", localtime(&cur_time));
    }
}

int main() 
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("socket() failed!");
        return 1;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(9000);

    if(bind(listener, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind() failed!");
        return 1;
    }
    
    if(listen(listener, 5) < 0) {
        perror("listen() failed!");
        return 1;
    }

    char buf[80];
    char *msg = "Wrong format, try again!\n";
    for(int i=0; i<8; i++) {
        if(fork() == 0) {
            while(1)
            {
                int client = accept(listener, NULL, NULL);
                printf("New client accepted %d\n", client);

                // nhan du lieu tu client
                int ret = recv(client, buf, sizeof(buf), 0);
                if(ret <= 0) {
                    continue;
                }
                buf[ret] = 0;
                while(check_format(buf) == 0) {
                    printf("Wrong data format from client\n");
                    send(client, msg, strlen(msg), 0);
                    ret = recv(client, buf, sizeof(buf), 0);
                    if(ret <= 0) {
                        continue;
                    }
                    buf[ret] = 0;
                }

                // xu ly du lieu 
                data_process(buf);
                send(client, buf, strlen(buf), 0);

                close(client);
            }
            exit(0);
        }
    }
    getchar();
    killpg(0, SIGKILL);
    return 0;
}