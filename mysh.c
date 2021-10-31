// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <errno.h>
#include <signal.h>

// Strusts
struct token
{
    int id;
    int num;
    char text[200];

} typedef token;


// Global variables
char tokens[][12] = {"movetodir", "whereami", "history", "-c", "byebye", "replay", "start", "background", "dalek"};
int hisIndex = 0, hisCap = 100;
char ** history;
char currentdir[200];


// Function prototypes
token tokenize(char name[12]);
int tokenizeCom(char commandStr[200], token commandTok[10], int length);
void reset(token commandTok[10], char commandStr[200]);
void executeCom(char commandStr[200], token commandTok[10], int length1);
void addCom(char commandStr[200]);
int isDirectoryExists(const char * path);
int fileExists(const char *fname);
void resetHis();
void start(const char *fname, char ** argv, int length);
void background(const char *fname, char ** argv, int length);

// Main function
int main()
{
    FILE *read;
    FILE *write;
    char commandStr[200], temp[200];;
    token commandTok[10];

    // Get current directory
    getcwd(currentdir, 200);

    // Initilize commandTok and commandStr arrays
    reset(commandTok, commandStr);


    // initilize history array
    history = malloc(hisCap * sizeof(char * ));
    
    for(int i = 0; i < hisCap; i++)
    {
        history[i] = malloc(200 * sizeof(char));
    }    

    // Check if history exists, and load it in if nessesary
    if(fileExists("history.txt"))
    {
        read = fopen("history.txt", "r");

        while(!feof(read))
        {
            fscanf(read, "%[^\n]%*c", temp);
            addCom(temp);
        }
    }

    // Generate command prompt, and process commands
    while(commandTok[0].id != 4)
    {
        reset(commandTok, commandStr);        
        printf("# ");
        reset(commandTok, commandStr);
        gets(commandStr);
        executeCom(commandStr, commandTok, strlen(commandStr));
    }

    // Register current history log, before exitting program
    write = fopen("history.txt", "w+");

    for(int i = 0; i < hisIndex; i++)
    {
        fprintf(write, "%s", history[i]);
        if(i != hisIndex - 1)
            fprintf(write, "\n");
    }

    return 0;
}

// Initilize history array
void resetHis()
{
    hisIndex = 0, hisCap = 100;   
    history = realloc(history, hisCap * sizeof(char *));
}

// Function that tokenizes a given symbol
token tokenize(char name[12])
{   
    token ret;
    for(int i = 0; i < 9; i++)
    {
        if(strcmp(name, tokens[i]) == 0)
        {
            ret.id = i;
            return ret;
        }
    }

    if(isdigit(name[0]) != 0)
    {
        ret.id = 11;
        ret.num = atoi(name);
        strcpy(ret.text, name);
    }
    else
    {
        ret.id = 12;
        strcpy(ret.text, name);
    }

    return ret;
}

// Function that tokenizes a given string
int tokenizeCom(char commandStr[200], token commandTok[10], int length)
{
    char temp[200];
    int comIndex = 0, i, reg = 0, tokIndex = 0;

    for(i = 0; i < 200; i++)
        temp[i] = '\0';

    for(i = 0; i < length; i++)
    {
        if(commandStr[i] != ' ' && i != length - 1)
        {
            if(reg == 0)
                reg = 1;

            temp[comIndex] = commandStr[i];
            comIndex++;
        }
        else if((commandStr[i] == ' ' || i == length - 1) && reg == 1)
        {
            if(i == length - 1)
                temp[comIndex] = commandStr[i];  

            commandTok[tokIndex] = tokenize(temp);
            tokIndex++;
            comIndex = 0;

            for(int j = 0; j < 200; j++)
                temp[j] = '\0';
        }
        else
            continue;
    } 

    return tokIndex;
}

// Function that resets given arrays
void reset(token commandTok[10], char commandStr[200])
{
    int i;
    for(i = 0; i < 10; i++)
    {
        commandTok[i].id = -1;
        commandTok[i].num = 0;
        
        for(int j = 0; j < 200; j++)
            commandTok[i].text[j] = '\0';
    }

    for(i = 0; i < 100; i++)
        commandStr[i] = '\0';
}

// Function that executes a given command
void executeCom(char commandStr[200], token commandTok[10], int length1)
{
    // Variables
    char temp[200];
    char ** args;
    int dirLen = strlen(currentdir), j;
    int length = tokenizeCom(commandStr, commandTok, length1);


    // Switch statement that switches executed code based on given command
    switch(commandTok[0].id)
    {
        case 0: // movetodir

            strcpy(temp, currentdir);            


            if(commandTok[1].id == 12 && strcmp(commandTok[1].text, "..") != 0)
            {
                addCom(commandStr);
                if(commandTok[1].text[0] == '/')
                {
                    if(isDirectoryExists(commandTok[1].text))
                        strcpy(currentdir, commandTok[1].text);
                    else
                        printf("Directory does not exists\n");
                }
                else if(isDirectoryExists(strcat(strcat(temp, "/"), commandTok[1].text)) || isDirectoryExists(strcat(temp, commandTok[1].text)))
                {
                    if(currentdir[dirLen - 1] == '/')
                        strcat(currentdir, commandTok[1].text);
                    else
                        strcat(strcat(currentdir, "/"), commandTok[1].text);
                }
                else
                    printf("Directory does not exists\n");

            }
            else if(strcmp(commandTok[1].text, "..") == 0)
            {
                addCom(commandStr);
                dirLen;
                while(currentdir[dirLen - 1] != '/')
                {
                    currentdir[dirLen - 1] = '\0';
                    dirLen--;
                }

                currentdir[dirLen - 1] = '\0';
                    dirLen--;
            }
            else
                printf("Unrecognized command.\n");

            break;
            
        case 1: // whereami

            printf("%s\n", currentdir);
            addCom(commandStr);

            break;

        case 2: // history

            addCom(commandStr);
            j = 0;

            if(length  > 1 && commandTok[1].id == 3)
            {
                resetHis();
            }
            else
            {
                for(int i = hisIndex - 1; i >= 0; i--)
                    printf("%d: %s\n", j++ , history[i]);
                    
            }
            break;

        case 4: // byebye

            addCom(commandStr);

            break;

        case 5:  // replay

            addCom(commandStr);
            j = 0;

            if(length > 1 && commandTok[1].id == 11)
            {
                for(int i = hisIndex - 1; i >= 0; i--)
                {
                    if(j - 1 == commandTok[1].num)
                    {
                        addCom(history[i]);
                        printf("# %s\n", history[i]);
                        int length = tokenizeCom(history[i], commandTok, strlen(history[i]));
                        executeCom(history[i], commandTok, strlen(history[i]));
                        return;
                    }
                    j++;
                }

                printf("Command not found.\n");
                    
            }
            else
            {
                printf("Unrecognized command.\n");
            }

            break;

        case 6: // start

            j = 1;

            if(length >= 2)
            {
                addCom(commandStr);
                args = malloc((length - 1) * sizeof(char *)); 
                
                for(int i = 0; i < length - 1; i++)
                    args[i] = malloc(200 * sizeof(char));
                
                for(int i = 2; i < length; i++ )
                    if(commandTok[i].id == 11 || commandTok[i].id == 12)
                        strcpy(args[j++], commandTok[i].text);


                start(commandTok[1].text, args, length - 1);
            }
            else
                printf("Unrecognized command.\n");


            break;

        case 7: // background

            j = 1;

            if(length >= 2)
            {
                addCom(commandStr);
                args = malloc((length - 1) * sizeof(char *)); 
                
                for(int i = 0; i < length - 1; i++)
                    args[i] = malloc(200 * sizeof(char));
                
                for(int i = 2; i < length; i++ )
                    if(commandTok[i].id == 11 || commandTok[i].id == 12)
                        strcpy(args[j++], commandTok[i].text);



                background(commandTok[1].text, args, length - 1);
            }
            else
                printf("Unrecognized command.\n");

            break;

        case 8: //dalek

            if(length >= 2 && commandTok[1].id == 11)
            {
                
                addCom(commandStr);

                if(kill(commandTok[1].num, 0) == -1 && errno == ESRCH)
                    printf("process does not exist\n");
                else 
                {
                    waitpid(commandTok[1].num, 0, WNOHANG);
                    kill(commandTok[1].num, SIGKILL);
                    if(kill(commandTok[1].num, 0) == -1 && errno == ESRCH)
                        printf("Termination successful.\n");
                    else
                        printf("Termination unsuccessful.\n");
                }
            }
            else
                printf("Unrecognized command.\n");
            
            break;
        default:
            printf("Unrecognized command.\n");
            break;
    }
}

// Add current command to history
void addCom(char commandStr[200])
{
    int j = 0;
    int temp = hisCap;


    // If history capacity is full, double it
    if(hisIndex >= hisCap)
    {
        hisCap = 2*hisCap;
        history = realloc(history, hisCap * sizeof(char *));


        for(int i = temp; i < hisCap; i++)
        {
            history[i] = malloc(200 * sizeof(char));
        }
    }

    if(temp = hisCap)
       strcpy(history[hisIndex], commandStr);

    hisIndex++;
}

// Check if a given directory exists
int isDirectoryExists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
}

// Check if a file exists
int fileExists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

// Function that runs a given process
void start(const char *fname, char ** argv, int length)
{
    char name[200];

    if(fname[0] == '/')
    {
        strcpy(name, fname);
    }
    else
    {
        strcpy(name, currentdir);
        strcat(name, "/");
        strcat(name, fname);
    }

    strcpy(argv[0], name);

    char * temp[length];

    for(int i = 0; i < length; i++)
        temp[i] = malloc(200 * sizeof(char));

    for(int i = 0; i < length; i++)
        strcpy(temp[i], argv[i]);


    pid_t pid=fork();

    if(pid==0) 
    {
        if(execvp(argv[0], temp) == -1)
            printf("Cannot find/run given file.\n");
        exit(127); 
    }
    else {
        waitpid(pid,0,0); 
    }
}

// Function that runs a given process in the background
void background(const char *fname, char ** argv, int length)
{
    char name[200];

    if(fname[0] == '/')
    {
        strcpy(name, fname);
    }
    else
    {
        strcpy(name, currentdir);
        strcat(name, "/");
        strcat(name, fname);
    }

    strcpy(argv[0], name);

    char * temp[length];

    for(int i = 0; i < length; i++)
        temp[i] = malloc(200 * sizeof(char));

    for(int i = 0; i < length; i++)
        strcpy(temp[i], argv[i]);

    pid_t pid=fork();

    if(pid==0) 
    {
        if(execv(argv[0], temp) == -1)
            printf("Cannot find/run given file.\n");
        exit(127); 
    }
    else {
        waitpid(pid, 0, WNOHANG);
        printf("PID = %d\n", pid);    
    }
}