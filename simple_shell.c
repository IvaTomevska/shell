#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>
#define delim " \t\r\n\a"


extern int alphasort(); //Inbuilt sorting function

#define FALSE 0
#define TRUE !FALSE

/* Read input */
char *read_line(void){
    char *buffer = NULL;
    size_t bufsize = 0;
    getline(&buffer, &bufsize, stdin);
    return buffer;
}

/* Find the current directory */
char *get_path(){
	static char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	return cwd;
}


/*Remove '.' and '..' files*/
int select_files(const struct direct *entry) {
	if((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
		return (FALSE);
	else
		return (TRUE);
}

int main() {

	/*Primary loop (it keeps asinkg for input until exit or Ctrl+D)*/
	while(!feof(stdin))
	{
		printf("$ ");

		/* Read the input and store into str */
		char *str = read_line();

		if(*str==NULL){
			break;
		}

		/* tokenize the string */
		char **tokens[64];
		char *token=NULL;
		int i = 0;
		token = strtok(str,delim);
		while (token != NULL){
			tokens[i] = token;
			i++;
			if (i == 64)
				tokens[i] ==NULL;
			token = strtok(NULL, delim);
		}
		tokens[i]=NULL;

		char *path_name;
		path_name = get_path();	//current dir
		
		/* Print current directory */
		if (strcmp(tokens[0],"pwd")==0){
			if(!path_name)
				perror("Error getting pathname\n");	//error
			else	
			printf("Current working directory: %s\n", path_name);	//print current dir
		}

		/* Change directory
		cd: show error
		cd ..: go to parent dir
		cd directory_name: go to new directory, report if does not exist
		*/
		else if (strcmp(tokens[0],"cd")==0){
			if(tokens[1]==NULL)
				printf("Missing argument\n");

			else if(strcmp(tokens[1],"..")==0){
				chdir("..");
				printf("Current working directory: %s\n", get_path());
				continue;
			}
			else{
				path_name = strcat(path_name, "/");
				path_name = strcat(path_name, tokens[1]);				
				if(chdir(path_name)==0)
					printf("Current working directory: %s\n", path_name);
				else
					printf("Command failed: no such file or directory\n");
			}
		}

		/* List out the files in the current directory */
		else if (strcmp(tokens[0], "ls") == 0) {
			struct direct **files;
			/*Scans directory, filters them through select_files,
			the files which don't return 0 are storred in struct files
			sorted with alphasort()*/
			int count = scandir(path_name, &files, select_files, alphasort);

			if(count<=0)
				perror("No files in this directory\n");

			//Print files in directory
			for (int i = 0; i < count; i++)
				printf("%s ", files[i]->d_name);
			printf("\n");

		}
		
		/* Exit the shell */
		else if (strcmp(tokens[0],"exit")==0 || tokens[0]==NULL)
			break;

		else	//if input does not make sense
			printf("This is not a proper command\n");
	}

	
	return 0;
}