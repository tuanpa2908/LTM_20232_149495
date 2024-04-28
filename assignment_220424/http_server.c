#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    int listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0) {
        perror("socket() failed");
        return 1;
    }
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(9000);

    if(bind(listener, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("bind() failed");
        return 1;
    }

    if(listen(listener, 5) < 0) {
        perror("listen() failed");
        return 1;
    }

    int num_processes = 8;
    char buf[256];
    for(int i = 0; i < num_processes; i++) {
        if(fork() == 0) {
            while (1)
            {
                int client = accept(listener, NULL, NULL);
                printf("New client accepted in process %d: %d\n", client, getpid());

                int ret = recv(client, buf, sizeof(buf), 0);
                if(ret <= 0)
                    continue;
                buf[ret] = 0;
                puts(buf);
                char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
                send(client, msg, strlen(msg), 0);

                close(client);
            }
            exit(0);
        }
    }
    getchar();
    killpg(0, SIGKILL);     
    return 0;
}