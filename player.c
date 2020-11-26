#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define MAXLINE 100000
#define SERV_PORT 9877

int main(int argc, char **argv)
{
	//message
    const char *login_msg = "Welcome to Tic-Tac-Toe game server\nPlease enter your account\n\tusername:";

	int					sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
	{
        printf("Need two argument.\n");
        return -1;
    }

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr));

	printf("%s",login_msg);

	char	sendline[MAXLINE], recvline[MAXLINE];

	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		
		//printf("client :%s\n",sendline);

		write(sockfd, sendline, strlen(sendline));

		if (read(sockfd, recvline, MAXLINE) == 0)
		{
			printf("Server terminated prematurely.\n");
			return 0;
		}

		//printf("server :");
		fputs(recvline, stdout);
		memset(recvline,0,strlen(recvline));
		//printf("\n");
	}

	exit(0);
}