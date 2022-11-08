/**
 * CS2106 AY22/23 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */


#include "myshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>

struct PCBTable* processTable;
int tableID = 0;

void updateStatus(pid_t pid, int status, int exitCode);
int preprocess(size_t num_tokens, char **tokens);
void my_info(int option);
void my_wait(pid_t pid);
void my_terminate(pid_t pid);
void my_wait_terminating(void);
bool myCheckDigit(char*);


void my_init(void) {
    // Initialize what you need here
    processTable = malloc(2 * MAX_PROCESSES * sizeof(struct PCBTable));
    for (int i = 0; i < 2 * MAX_PROCESSES; i++){
        processTable[i].pid = -1;
    }
}



void my_process_command(size_t num_tokens, char **tokens) {

    // Your code here, refer to the lab document for a description of the
    // arguments

    pid_t pid;

    size_t ret_argv_nmemb = 8;
    char** args = calloc(ret_argv_nmemb, sizeof(char *));

    size_t tokenIndex = 0;
    size_t argIdx = 0;
    bool backgroundProcess = false;

    char *inputFile, *outputFile, *errFile;
    int finput, foutput, ferr;
    bool inputRedirection = false, outputRedirection = false, errRedirection = false;
    while (tokenIndex < num_tokens){
        if (tokens[tokenIndex] == NULL || strcmp(tokens[tokenIndex], ";") == 0)
        {
            args[argIdx] = NULL;
            if (strcmp(args[0], "info") == 0 ){
                int option;
                if (! myCheckDigit(args[1])){
                    printf("Wrong command\n");
                }
                else{
                    option = atoi(args[1]);
                    my_info(option);
                }
            }
            else if (strcmp(args[0], "wait") == 0){
                int pid;
                if (! myCheckDigit(args[1])){
                    printf("Wrong command\n");
                }
                else{
                    pid = atoi(args[1]);
                    my_wait(pid);
                }
            }
            else if (strcmp(args[0], "terminate") == 0){
                int pid;
                if (! myCheckDigit(args[1])){
                    printf("Wrong command\n");
                }
                else{
                    pid = atoi(args[1]);
                    my_terminate(pid);
                }
            }
            else{
                if (access(args[0], X_OK | R_OK) == -1){
                    fprintf(stderr, "%s not found\n", args[0]);
                    tokenIndex++; // Since using continue, move to next ptr
                    continue;
                } 
                pid = fork();

                if (pid == -1){
                    fputs("DEBUGGING: Unable to fork a child process", stdout);
                }
                else if (pid == 0){  // Child process to execute function
                    
                    if (inputRedirection){
                        finput = open(inputFile, O_RDONLY | O_CLOEXEC);
                        if (finput == -1){
                            fprintf(stderr, "%s does not exist\n", inputFile);
                            exit(1);
                        }
                        else
                            dup2(finput, STDIN_FILENO);
                    }

                    if (outputRedirection){
                        foutput = open(outputFile, O_WRONLY | O_CLOEXEC | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR );
                        if (foutput == -1)
                            fprintf(stderr, "%s cannot be opened\n", outputFile);
                        else
                            dup2(foutput, STDOUT_FILENO);
                    }
                    

                    if (errRedirection){
                        ferr = open(errFile, O_WRONLY | O_CLOEXEC | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                        if (ferr == -1)
                            fprintf(stderr, "%s cannot be opened\n", errFile);
                        else
                            dup2(ferr, STDERR_FILENO);
                    }

                    
                    if (execv(args[0], args) == -1) {
                        fprintf(stderr, "Error executing input program %s\n", args[0]);
                    }

                    return;
                }
                else{
                    updateStatus(pid, 2, -1); // Might have logical error if exec fails
                    if (backgroundProcess){
                        printf("Child [%u] in background\n", pid);
                        backgroundProcess = false;
                    } 
                    else{
                        my_wait(pid);
                    }
                }
            }
            argIdx = 0;
            ret_argv_nmemb = 8;
            args = realloc(args, ret_argv_nmemb * sizeof(char *));
            inputRedirection = false;
            outputRedirection = false;
            errRedirection = false;
        }
        else if(strcmp(tokens[tokenIndex], "<") == 0){
            inputRedirection = true;
            inputFile = tokens[++tokenIndex];
        } 
        else if (strcmp(tokens[tokenIndex], ">") == 0){
            outputRedirection = true;
            outputFile = tokens[++tokenIndex];
        }
        else if(strcmp(tokens[tokenIndex], "2>") == 0){
            errRedirection = true;
            errFile = tokens[++tokenIndex];
        }
        else if(strcmp(tokens[tokenIndex], "&") == 0){
            backgroundProcess = true;
        }
        else{
            
            if (argIdx + 1 >= ret_argv_nmemb){
                ret_argv_nmemb += 8;
                args = realloc(args, ret_argv_nmemb * sizeof(char *));
                if (!args) {
                    goto fail;
                }
            }
            args[argIdx] = tokens[tokenIndex];
            argIdx++;
        }

        tokenIndex++; // Moving forward
    }
        

fail:
    if (args) {
        free(args);
    }
    return;
 
}

void my_info(int option){
    my_wait_terminating();
    if (option == 0){
        for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
            printf("[%u] ", processTable[i].pid);
            if (processTable[i].status == 1){
                printf("Exited %d\n", processTable[i].exitCode);
            }
            else if (processTable[i].status == 2){
                printf("Running\n");
            }
            else if (processTable[i].status == 3){
                printf("Terminating\n");
            }
        }
    }
    else if (option == 1){
        int nExited = 0;
        for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
            if (processTable[i].status == 1)
                nExited++;
        }
        printf("Total exited process: %d\n", nExited);
    }
    else if (option == 2){
        int nRunning = 0;
        for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
            if (processTable[i].status == 2)
                nRunning++;
        }
        printf("Total running process: %d\n", nRunning);
    }
    else if (option == 3){
        int nTerminating = 0;
        for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
            if (processTable[i].status == 3)
                nTerminating++;
        }
        printf("Total terminating process: %d\n", nTerminating);
    }
    else{
        printf("Wrong command\n");
    }
}

void my_wait(pid_t pid){
    int status = 0;
    for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
        if (processTable[i].pid == pid && processTable[i].status == 2){ // Process is running
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)){
                processTable[i].exitCode = WEXITSTATUS(status);
                processTable[i].status = 1;
            }
            else if(WIFSIGNALED(status)){
                processTable[i].exitCode = WTERMSIG(status);
                processTable[i].status = 1;
            }
        }             
    }
}

void my_terminate(pid_t pid){
    for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
        if (processTable[i].pid == pid && processTable[i].status == 2){
            kill(processTable[i].pid, SIGTERM);
            processTable[i].status = 3;
        }
                
    }
}

void my_quit(void) {

    // Clean up function, called after "quit" is entered as a user command
    my_wait_terminating();
    for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
            if (processTable[i].status == 2){
                kill(processTable[i].pid, SIGTERM);
                printf("Killing [%u]\n", processTable[i].pid);
            }
                
    }
    free(processTable);
    printf("\nGoodbye\n");
}

void my_wait_terminating(void){ // Non-blocking function to check for terminating states
    int status;
    for (int i = 0; i < MAX_PROCESSES && processTable[i].pid != -1; i++){
        if (processTable[i].status == 3 || processTable[i].status == 2){
            if (waitpid(processTable[i].pid, &status,  WNOHANG) > 0){
                if (WIFEXITED(status)){
                    processTable[i].exitCode = WEXITSTATUS(status);
                    processTable[i].status = 1;
                }
                if(WIFSIGNALED(status)){
                    processTable[i].exitCode = WTERMSIG(status);
                    processTable[i].status = 1;
                }
            }
        }
    }
}

void updateStatus(pid_t pid, int status, int exitCode){
    processTable[tableID].pid = pid;
    processTable[tableID].status = status;
    processTable[tableID].exitCode = exitCode;
    tableID++;
}

bool myCheckDigit(char* args){
    // printf("Checking validity of mydigit\n");
    if (args == NULL) return false;
    for (int i = 0; args[i] != '\0'; i++){
        if (isdigit(args[i]) == 0)
            return false;
    }
    return true;
}

void print_args(char** args){
    printf("List of arguments: ");
    for (int i = 0; args[i] != NULL; i++){
        printf("%s ", args[i]);
    }
    printf("\n");
}