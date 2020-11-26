#include "finclude.h"

int main(int argc, char **argv)
{

	int					i, j, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready;// client[0][]:login status  client[1][]:connect fd  client[2][]:client index
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE],reply[MAXLINE],reply_rival[MAXLINE];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

    printf("OOXX server: v0.4.0\n");

    //Server setting
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr_in *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
    {
		client[LOG_STAT][i] = -1;		/* -1 indicates available entry */
        client[CONNFD][i] = -1;
        client[USER_IDX][i] = -1;  
    }

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

/* Get player account */
    printf("Player infomation loading...\n");
    int player_num;
    player_num = load_player();
    printf("Account number: %d\n",player_num);
    listPlayerInfo(player_num);
    listClient();

/* Service */
	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);


        /* new client connection */
        // accept the connection and send login message
		if (FD_ISSET(listenfd, &rset)) {	
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (struct sockaddr_in *) &cliaddr, &clilen);
#ifdef	NOTDEF
			printf("new client: %s, port %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
#endif

			for (i = 0; i < FD_SETSIZE; i++)
				if (client[CONNFD][i] < 0) {
					client[CONNFD][i] = connfd;	/* save descriptor */
                    client[LOG_STAT][i] = LOGIN_USER;
					break;
				}
			if (i == FD_SETSIZE)
            {
                //Client full. Can't accept more
                printf("Too many client. Discard client: %s, port %d\n",
					inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
                continue;
            }

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[CONNFD][i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) {
				if ( (n = read(sockfd, buf, MAXLINE)) == 0) {
						/*connection closed by client */
                    printf("connection closed by client.\n");
					close(sockfd);
					FD_CLR(sockfd, &allset);
                    list[client[USER_IDX][i]].status = -1;
                    client[LOG_STAT][i] = LOGIN_INIT;
					client[CONNFD][i] = -1;
                    client[USER_IDX][i] = -1;
				}
                else // a valid client message: process it
				{
                    //printf("n :%d\n",n);
                    buf[n-1]=0;
                    printf("000 client message:\n%s\n",buf);

                    //login: uaername
                    if(client[LOG_STAT][i]==LOGIN_USER)
                    {
                        for(j=0;j<player_num;j++)
                            if(!strcmp(list[j].username,buf))
                            {
                                if(list[j].status == -1)
                                {
                                    client[LOG_STAT][i] = LOGIN_PASS;
                                    client[USER_IDX][i] = j;
                                    write(client[CONNFD][i],"\tpassword:",11);
                                    break;
                                }
                                else
                                {
                                    write(client[CONNFD][i],"This account has login!! Please try again.\n\tusername :",55);
                                    break;
                                }
                            }

                        if(j==player_num)
                            write(client[CONNFD][i],"Can't identified your username. Please try again.\n\tusername :",62);
                        
                    }

                    //login: password
                    else if(client[LOG_STAT][i]==LOGIN_PASS)
                    {
                        if(!strcmp(list[client[USER_IDX][i]].password,buf))
                        {
                            client[LOG_STAT][i] = LOGIN_SUCCESS;
                            sprintf(reply,"Welcome %s\n\t>>",list[client[USER_IDX][i]].ID);
                            list[client[USER_IDX][i]].status = sockfd;
                            write(client[CONNFD][i],reply,strlen(reply));
                        }
                        else
                            write(client[CONNFD][i],"Wrong password, please try again.\n\tpassword :",46);
                    }

                    //other service after login
                    else
                    {

                        //help
                        if(!strncmp(buf,"help",4))
                        {
                            sprintf(reply,"Instruction manual:\n\t");
                            strcat(reply,"list:\tlist all the online-player\n\t");
                            strcat(reply,"logout:\tlogout the game\n\t");
                            strcat(reply,"match i\t:ask \"i\" to play with you(i is an integer, your rival's ID)\n\t>>");
                            strcat(reply,"\n\t>>");
                            write(client[CONNFD][i],reply,strlen(reply));
                        }

                        //logout: logout "this" account. allow client to login again
                        else if(!strncmp(buf,"logout",6))
                        {
                            client[LOG_STAT][i] = LOGIN_USER;
                            list[client[USER_IDX][i]].status = -1;
                            client[USER_IDX][i] = -1;
                            sprintf(reply,"Logout complete.\nSee you next time...\n\tusername:");
                            write(client[CONNFD][i],reply,strlen(reply));
                        }

                        //list: list all the online player
                        else if(!strncmp(buf,"list",4))
                        {
                            sprintf(reply,"ONLINE:\n\t");
                            char *appender = malloc(APPEND_LINE * sizeof(char));
                            for(j=0;j<player_num;j++)
                            {
                                if(list[j].status!=-1)
                                {
                                    sprintf(appender,"%d\t%s\n\t",list[j].status,list[j].ID);
                                    strcat(reply,appender);
                                }
                            }
                            strcat(reply,"\nOFFLINE:\n\t");
                            for(j=0;j<player_num;j++)
                            {
                                if(list[j].status==-1)
                                {
                                    sprintf(appender,"%d\t%s\n\t",list[j].status,list[j].ID);
                                    strcat(reply,appender);
                                }
                            }
                            strcat(reply,"\n\t>>");
                            write(client[CONNFD][i],reply,strlen(reply));
                            free(appender);
                        }

                        // match: ask for a challenge
                        else if(!strncmp(buf,"match",5))
                        {
                            char *match = malloc(MATCH * sizeof(char));
                            int asker = -1;
                            int rival = -1; // client(reply) user index
                            int v,s; //v:client(ask) connfd  s:client(reply) connfd
                            v = client[CONNFD][i];
                            sscanf(buf,"%s%d",match,&s);
                            if(s==v)
                                write(client[CONNFD][i],"You can't play with yourself.\n\t>>",34);
                            else
                            {
                                //printf("%s %d",match,rival);
                                for(j=0;j<player_num;j++)
                                {
                                    if(list[j].status == v)
                                        asker = j;
                                    else if(list[j].status == s)
                                        rival = j;
                                    else
                                        continue;
                                    
                                }
                                if(rival == -1 || asker == -1)
                                    write(client[CONNFD][i],"Can't find the player's ID.\n\t>>",32);
                                else
                                {
                                    list[asker].gamestat = GAME_ASK;
                                    list[rival].gamestat = GAME_ASK;
                                    sprintf(reply,"Asking %s to play with you...",list[rival].ID);
                                    write(v,reply,strlen(reply));
                                    sprintf(reply,"%s : I want to OOXX with you ~ ~\n(Y/N)",list[asker].ID);
                                    write(s,reply,strlen(reply));
                                }
                            }
                        }

                        else
                        {
                            write(client[CONNFD][i],"\t>>",4);
                        }
                        
                    }
                    
                    // reset buffer
                    memset(buf,0,strlen(buf));
                    memset(reply,0,strlen(buf));
                }
                

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
        // update player information
        printf("\n\n\n");
        listPlayerInfo(player_num);
        listClient();
	}
}