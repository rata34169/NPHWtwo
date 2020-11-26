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
#define LISTENQ 1024

#define NOTDEF

#define LOG_STAT 0
#define CONNFD 1
#define USER_IDX 2

#define LOGIN_INIT -1
#define LOGIN_USER 0
#define LOGIN_PASS 1
#define LOGIN_SUCCESS 2

#define APPEND_LINE 1000
#define MATCH 10

// Game status
#define AVALIBLE -1
#define GAME_ASK 0
#define GAME_X 1
#define GAME_O 2

// Rival
#define NONE -1

#include "fplayerinfo.c"
