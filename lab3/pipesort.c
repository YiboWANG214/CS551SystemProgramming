/*
THIS CODE IS MY OWN WORK, IT WAS WRITTEN WITHOUT CONSULTING
A TUTOR OR CODE WRITTEN BY OTHER STUDENTS – Yibo WANG
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void fork_sorters(int num_sort, int (*parse_fd)[2], int (*sort_fd)[2], int *pid)
{
	int i;
    for(i = 0; i < num_sort; i++) {
        pipe(parse_fd[i]);
        pipe(sort_fd[i]);
    }

	for (i = 0; i < num_sort; i++) {
        pid[i] = fork();
        if (pid[i] != -1) {
            if (pid[i] == 0) {
                int j;
                for (j = 0; j < num_sort; j++) {
                    if (i != j) {
                    	close(parse_fd[j][0]);
                    	close(sort_fd[j][1]);
                    }
                }
                for (j = 0; j < num_sort; j++) {
                	close(parse_fd[j][1]);
                	close(sort_fd[j][0]);
                }
                if (dup2(parse_fd[i][0], STDIN_FILENO) != -1) {
                    if (dup2(sort_fd[i][1], STDOUT_FILENO) != -1) {
                        execlp("sort", "sort", NULL);
                        close(parse_fd[i][0]);
                        close(sort_fd[i][1]);
                    }
                    else {
                        printf("dup sort fd error\n");
                        exit(1);
                    }
                }
                else {
                    printf("dup parse fd error\n");
                    exit(1);
                }
            }
        }
        else {
            printf("create process error\n");
            exit(1);
        }
	}
}

void parse_input(int num_sort, int (*parse_fd)[2], int (*sort_fd)[2], int l, int s)
{
	char c;
	char buf[l+2];
	int count=0;
	FILE* parser_out[num_sort];

	int i;
	for (i = 0; i < num_sort; i++) {
		close(sort_fd[i][1]);
		close(parse_fd[i][0]);
		parser_out[i] = fdopen(parse_fd[i][1], "w");
		if (parser_out[i] == NULL) {
			printf("parse write error\n");
			exit(1);
		}
	}

	i = 0;
	while((c=fgetc(stdin)) != EOF) {
		if (isalpha(c)){
			if (count < l){
				buf[count++] = tolower(c);
			}
		}
		else{
			if (count>s){
				buf[count++] = '\0';
				fputs(buf, parser_out[i]);
				fputs("\n", parser_out[i]);
				if (i == (num_sort - 1))
					i = 0;
				else
					i++;
			}
			count = 0;
			buf[0] = '\0';
		}
	}
	if (count>=s){
		buf[count++] = '\0';
		fputs(buf, parser_out[i]);
		fputs("\n", parser_out[i]);
	}

	for (i = 0; i < num_sort; i++) {
		fclose(parser_out[i]);
	}

}

void fork_merger(int num_sort, int (*sort_fd)[2], int (*parse_fd)[2], int l)
{
    int cpid = fork();
    int i;

    if (cpid < 0){
    	printf("merge error\n");
    	exit(1);
    }
    else if (cpid == 0){
    	for(i = 0; i < num_sort; i++) {
    		close(parse_fd[i][0]);
    		close(parse_fd[i][1]);
    		close(sort_fd[i][1]);
        }

        char words[num_sort][l+2];
        FILE* parser_out[num_sort];

        int n_empty = 0;
        for(i = 0; i < num_sort; i++) {
            parser_out[i] = fdopen(sort_fd[i][0], "r");
            if (fgets(words[i], l+2, parser_out[i]) == NULL){
	            words[i][0] = '\0';
	            n_empty++;
	        }
        }

        int next_word = -1;
        for (i = 0; i < num_sort; i++) {
        	if (words[i][0] != '\0'){
        		next_word = i;
        		break;
        	}
        }
        for (i = next_word + 1; i < num_sort; i++) {
            if (words[i][0] == '\0' || strcmp(words[i], words[next_word]) >= 0) 
            	continue;
            next_word = i;
        }

        char word[l+2];
        strcpy(word, words[next_word]);

        int n_repeat = 1;
        while(n_empty < num_sort) {
            if (fgets(words[next_word], l+2, parser_out[next_word]) != NULL) {}
            else {
                n_empty++;
                words[next_word][0] = '\0';
            }
            next_word = -1;
            for (i = 0; i < num_sort; i++) {
                if (words[i][0] != '\0'){
                	next_word = i;
                	break;
                }
            }
            for (i = next_word + 1; i < num_sort; i++) {
                if (words[i][0] == '\0' || strcmp(words[i], words[next_word]) >= 0) 
                	continue;
                next_word = i;
            }
            if (next_word != -1) {
                if (strcmp(word, words[next_word]) != 0) {
                    printf("%-10d%s", n_repeat, word);
                    strcpy(word, words[next_word]);
                    n_repeat = 1;
                }
                else n_repeat++;
            }
            else break;
        }
        printf("%-10d%s", n_repeat, word);
        for(i = 0; i < num_sort; i++) fclose(parser_out[i]);
    }
    else{
    	waitpid(cpid, NULL, 0);
    }
}

int main(int argc, char *argv[])
{
	int n = atoi(argv[2]);
	int s = atoi(argv[4]);
	int l = atoi(argv[6]);

	int parse_fd[n][2];
	int sort_fd[n][2];
	pid_t pid[n];

	int i;

	fork_sorters(n, parse_fd, sort_fd, pid);

	parse_input(n, parse_fd, sort_fd, l, s);
	
	fork_merger(n, sort_fd, parse_fd, l);

	for (i = 0; i < n; i++) {
			wait(NULL);
	}
	return 0;
}