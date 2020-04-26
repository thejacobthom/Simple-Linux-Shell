#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>

void ctrl_c(int i){
    signal(SIGINT, ctrl_c);
    printf("\nto terminate SEEsh, use Control-D\n\n? ");
    fflush(stdout);
}

char *read_line(){
	char *input = NULL;
	ssize_t buffer = 0;
	getline(&input, &buffer, stdin);
	return input;

}

char *command_list[] = {
    "cd",
    "pwd",
    "help",
    "exit",
    "set",
    "unset"
};


int command_amount(){
    return 6;
}


int cd(char **tokens){
    if(!(tokens[1] == NULL)){
        if(chdir(tokens[1]) !=0){
            printf("no such directory\n");
        }
    }
    return 1;
}

int pwd(char **tokens){
    char dir[PATH_MAX];
    if (getcwd(dir, sizeof(dir)) != NULL){
        printf("%s\n", dir);
         
    }
    else{
        printf("getcwd error \n");
        return 0;
    }
    return 1;
}

int help(char **tokens){
    printf("\nWelcome to SHEEsh by Jacob Thom V00892893\n");
    printf("  the following commands are built into  \n");
    printf("               this shell.               \n\n");
    
    for(int i = 0; i < command_amount(); i++){
        printf("%d: %s\n", i, command_list[i]);
    }
}

int exit_sh(char **tokens){
    return 0;
}

int set(char **tokens){
    if (tokens[1] != NULL){ // check for set
        if(strcmp(tokens[1], "var") == 0){
            if(tokens[2] != NULL){ // name of var
                if (tokens[3] != NULL){
                 setenv(tokens[2], tokens[3], 1);
                }
                else{
                    unsetenv(tokens[2]);
                }
            }
            else{
                printf("usage: set var [VALUE]\n");
            }
        }
    }
    else{
        char *tada = "printenv";
        system(tada);
    }
    return 1;
}  

int unset(char **tokens){
    if (tokens[1] != NULL){
        if (strcmp(tokens[1], "var") == 0){
            if (tokens[2] != NULL){
                unsetenv(tokens[2]);
            }
            else{
                printf("usage: unset var [NAME]\n");
            }
        }
    }
    else{
        printf("usage: unset var [NAME]\n");
    }
    
}

int (*command[])(char **) = {
    &cd,
    &pwd,
    &help,
    &exit_sh,
    &set,
    &unset
};


char **split_line(char *input){
    int buffer_inc = 100;
    int buffer = 100;
    int pos = 0;
    
    char **tokens = malloc(buffer * sizeof(char*));
    char *cur;
    
    if(!tokens){
        printf("Memory error 1\n");
        exit(0);
    }
    cur = strtok(input, " \t\r\a\n");
    while (cur != NULL){
        tokens[pos] = cur;
        pos++;
        
        if (pos >= buffer){
            buffer+=buffer_inc;
            tokens = realloc(tokens, buffer * sizeof(char*));
            if(!tokens){
                printf("Memory error 2\n");
                exit(0);
            }
        }
        cur = strtok(NULL, " \t\r\a\n");
    }
    
    tokens[pos] = NULL;
    return tokens;
    
}

int launch(char **tokens){
    pid_t pid, w_pid;
    int status;
    
    pid = fork();
    
    if (pid == 0){
        if (execvp(tokens[0], tokens) == -1){
            perror("error encountered");
        }
    }
    else if (pid < 0){
            perror("error encountered");
    }
    else{
    
        do{
            w_pid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int execute(char **tokens){

        if (tokens[0] == NULL) {
            return 1;
        }
        for (int i = 0; i < command_amount(); i++){
            if (strcmp(tokens[0], command_list[i]) == 0){
                return (*command[i])(tokens);
            }
        }
    return launch(tokens);
}



void clean(char *line, char **tokens, char **history){
    free(line);
    free(tokens);
}

void execute_rc(){
    char rc[256];
    char line[256];
    char **tokens;
    
    strcat(strcpy(rc, getenv("HOME")), "/.SEEshrc");
    FILE* rc_file = fopen(rc,"r");
    if (rc_file == NULL){
        printf("unable to open or locate .SHEEshrc\n");
    }
    else{
        while(fgets(line, sizeof(line), rc_file) != NULL){
            printf("%s", line);
            tokens = split_line(line);
            execute(tokens);
        }
        fclose(rc_file);
        printf("\nSEEshrc executed successfully!\n");
    }
}
void shellLoop(){
	int status = 1;
	char *input;
	char **tokens;
	char **history;
    
    signal(SIGINT, ctrl_c);
    
    // open rc file!
    execute_rc();
    printf("\nWelcome to the SEEsh Shell!\n");
    
	while(status){
		printf("? ");
		input = read_line();
        
        if(feof(stdin)){ //ctrl d
            printf("\n");
            exit(0);
        }
		
		tokens = split_line(input);
		status = execute(tokens);
		clean(input, tokens, history);
	}
}

int main(int argc, char **argv){
	shellLoop();

	printf("Exiting\n");

	return 0;
}


