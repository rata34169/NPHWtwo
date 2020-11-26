#define UP_LEN 20
#define MAX_PLAYER 10

typedef struct player
{
    char ID[UP_LEN];
    char username[UP_LEN];
    char password[UP_LEN];
    int status;             //offline: 0
                            //online: client port number
    int gamestat;
    int opponent;
};

//global account information
struct player list[MAX_PLAYER];
int client[3][FD_SETSIZE];

struct player initPlayer(char [],char [],char [],int );

int load_player()
{
  FILE *fp = fopen("account.txt", "rt");
  char ID[UP_LEN],username[UP_LEN],password[UP_LEN];
  int i;

  if (fp == NULL)
  {
    perror("Can't read file \"account.txt\".\n");
    exit(EXIT_FAILURE);
  }
  else
  {
    for(i=0 ; fscanf(fp, "%s %s %s", ID, username, password) != EOF ; i++)
      list[i] = initPlayer(ID , username, password, -1);
    fclose(fp);
  }
  return i;
}

struct player initPlayer(char ID[],char username[],char password[],int status)
{
    struct player temp;
    strcpy(temp.username, username);
    strcpy(temp.password, password);
    strcpy(temp.ID, ID);
    temp.status = status;
    temp.gamestat = AVALIBLE;
    temp.opponent = NONE;
    return temp;
}

void listPlayerInfo(int player_num)
{
    int i;
    for(i=0;i<player_num;i++)
    {
        printf("  ID      :%s\n  username:%s\n  password:%s\n  status  :%d\n  gamestat:%d\n  opponent:%d\n\n",list[i].ID, list[i].username, list[i].password, list[i].status, list[i].gamestat, list[i].opponent);
        //printf("  logstat :%d\n  connfd  :%d\n  cliidx  :%d\n\n",client[LOG_STAT][list[i].status],client[CONNFD][list[i].status],client[USER_IDX][list[i].status]);
    }
}

void listClient()
{
    printf("\n\n");
    int i;
    for(i=0;i<5;i++)
    {
        printf("client[%d]",i);
        printf("  logstat :%d\n  connfd  :%d\n  cliidx  :%d\n\n",client[LOG_STAT][i],client[CONNFD][i],client[USER_IDX][i]);
    }
}