/*
 * Purpose: Program to demonstrate the popen function.
 *
 * to do: Check that the 'popen' was successfull.
 *
 * Author:  M J Leslie.
 * Date:    08-Jan-94
*/
#include <iso646.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIZE_EXE 256


int giveSizeTerminal(){
    FILE *fp;
    char col[64];

    fp = popen("tput cols", "r");
    fgets( col, sizeof col, fp);
    int num = atoi(col);

    return num;
}
int getHostname(char hostname[]){

    if (gethostname(hostname, SIZE_EXE) != 0) {
        perror("gethostname failed");
        return 1;
    }
    return 0;
}

int getUptime(int *birthInstall, int *currentDate){
    int timeProgression = *currentDate - *birthInstall;
    timeProgression = timeProgression / 86400;
    return timeProgression;
}

int getFetchInfo(char kernel[], char package[], char shell[], int *day,char hostname[]){
    FILE *fp;
    fp = popen("pacman -Q | grep 'linux-hardened '", "r");
    fgets(kernel, SIZE_EXE ,fp);
    kernel[strcspn(kernel, "\n")] = '\0';

    fp = popen("pacman -Q | wc -l", "r");
    fgets(package, SIZE_EXE ,fp);
    package[strcspn(package, "\n")] = '\0';

    fp = popen("pacman -Q | grep 'zsh '", "r");
    fgets(shell, SIZE_EXE ,fp);
    shell[strcspn(shell, "\n")] = '\0';

    char chBirth[SIZE_EXE];
    char chCurrent[SIZE_EXE];
    fp = popen("stat -c %W /", "r");
    fgets(chBirth, SIZE_EXE, fp);
    fp = popen("date +%s", "r");
    fgets(chCurrent, SIZE_EXE, fp);
    chBirth[strcspn(chBirth, "\n")] = '\0';
    chCurrent[strcspn(chCurrent, "\n")] = '\0';


    int birthInstall = atoi(chBirth);
    int currentDate = atoi(chCurrent);
    *day = getUptime(&birthInstall,&currentDate);

    getHostname(hostname);

    return 0;
}

int main(void){
  FILE *fp;
  int numOfCols = giveSizeTerminal();
  char kernel[SIZE_EXE];
  char package[SIZE_EXE];
  char shell[SIZE_EXE];
  char hostname[SIZE_EXE];
  char* username = getenv("USER");
  int day;


  getFetchInfo(kernel, package, shell, &day,hostname);

  printf("%s@%s ", username, hostname);
  printf("| 󰣇  Linux %s ",kernel);
  printf("| 󰏖  %s (pacman) ",package);
  printf("|    %s ",shell);
  printf("|   %d day",day); fflush(stdout);


  printf("\n");

  return 0;
}
