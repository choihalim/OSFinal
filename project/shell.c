#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_LENGTH 1024
#define DELIMS " \t\r\n"

static const char n[] = "\n";
static const char r[] = "\r";
static const char t[] = "\t";

int count_lines (const char *str) {
    int lines=0, i=0;
        for(i=0; str[i] != '\0'; i++){
                if( str[i] == '\n' ){
                        lines+=1;
                }
        }
        return lines;
}
int count_words(const char *str) {
    int words = 0, j = 0;
    for (j=0; &str[j] != NULL;) {
        if (str[j] == ' ' || strcmp(&str[j], n) == 0 || (strcmp(&str[j], r)) == 0 || (strcmp(&str[j], t)) == 0 ) {
            j++;
        } else {
            break;
        }
    }
    for(; str[j] != '\0'; j++){
        if(str[j] == ' ' || strcmp(&str[j], n) == 0 || strcmp(&str[j], r) == 0 || strcmp(&str[j], t) == 0 ){
            words+=1;
            // skip consucative white space
            while(str[j]==' ' || str[j] == '\n' || str[j]== '\r' || str[j] == '\t'){
                j++;
            }
        }
    }
    return words;
}

void txt_wc(FILE *fp, int *line, int *word, int *c, char *name) {
    int lines = 0, words = 0 , chars = 0;
    char buffer[1024];
    size_t bytes;
    if (fseek(fp, 0L, SEEK_SET) != 0) {
        printf("ERROR: check if file/directory exists");
    }
    do {
        memset(buffer, 0x00, sizeof(buffer));
        bytes = fread(buffer, sizeof(char), sizeof(buffer)-1, fp);
        if(bytes > 0){
            lines += count_lines(buffer);
            words += count_words(buffer);
            chars += bytes;
        } 
    } while(bytes > 0);
    *line = lines;
    *word = words;
    *c = chars;
    printf("  %d  %d  %d  %s\n", lines, words, chars, name);
}

void read_content(char *file) {
    int fd, count;
    char buffer[2048];
    fd = open(file, O_RDONLY);
    if(fd == -1) {
        printf("Error: opening file");
    }
    while ((count = read(fd, buffer, sizeof(buffer))) > 0) {
        printf("%s", buffer);
    }
}

int main(int argc, char *argv[]) {
  char *cmd;
  char line[MAX_LENGTH];
  char *args_dup[MAX_LENGTH];
  int index = 0;
  int in, out;

  while (1) {
      int redirect;
      char path[200];
      getcwd(path, 200);
      printf("%s$ ", path);
    if (!fgets(line, MAX_LENGTH, stdin)) break;

    if ((cmd = strtok(line, DELIMS))) {
        errno = 0;

        if (strcmp(cmd, "cd") == 0) {
            char *arg = strtok(0, DELIMS); 

            if (!arg) fprintf(stderr, "cd missing argument.\n");
            else chdir(arg);

        } else if (strcmp(cmd, "exit") == 0) {
            break;

        } 
        if (strcmp(cmd, "rm") == 0) {
            cmd = strtok(NULL, DELIMS);
            if (remove(cmd) != 0) {
                printf("Error: No such file or directory\n");
            }
            continue; 
        } 
        if (strcmp(cmd, "wc") == 0) {
            FILE *fp = NULL;
            int lines = 0, words = 0, chars = 0;
            cmd = strtok(NULL, DELIMS);
            if (cmd == NULL) {
                printf("USAGE: wc <filename> \n");
            }
            fp = fopen(cmd, "r");
            txt_wc(fp, &lines, &words, &chars, cmd);
            fclose(fp);
            continue;
        }
        if (strcmp(cmd, "mkdir") == 0) {
            cmd = strtok(NULL, DELIMS);
            char *name = cmd;
            int fail = mkdir(name, 0777);
            if (fail) {
                printf("Error: Failed to create directory\n");
            }
        }
        if (strcmp(cmd, "cat") == 0) {
            cmd = strtok(NULL, DELIMS);
            read_content(cmd);
        }
        // while (cmd != NULL) {
        if (strcmp(cmd, "&") == 0) {
            printf("[%d]\n", getpid());
        }
            // cmd = strtok(NULL, DELIMS);
        // }
        
        if (strcmp(cmd, "<") == 0 || strcmp(cmd, ">") == 0) {
            redirect = 1;
        }
        
        if (redirect) {
            int input = strcmp(cmd, "<");
            int output = strcmp(cmd, ">");
            while(cmd != NULL) {
                if (input) {
                    cmd = strtok(NULL, DELIMS);
                    int fd0 = open(cmd, O_RDONLY, 0);
                    dup2(fd0, STDIN_FILENO);
                    close(fd0);
                    input = 0;
                    continue;
                }
                if (output) {
                    cmd = strtok(NULL, DELIMS);
                    cmd = strtok(NULL, DELIMS);
                    int fd1 = creat(cmd, 0644);
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);
                    output = 0;
                    continue;
                }
                cmd = strtok(NULL, DELIMS);
            }
        }
        if (system(line) == -1) {
            printf("Error: system() function call\n");
        }
    }
        
    }
  return 0;
}