#include <asm-generic/ioctls.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define SIZE_EXE 256

int giveSizeTerminal(){
    struct winsize w;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
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

char* cutAfterSpace(const char str[], char result[]) {
    char* space_ptr = strchr(str, ' ');
    const char* after_space = space_ptr + 1;

    strcpy(result, after_space);
    return result;
}


void run_command(const char *cmd, char *output, size_t size) {
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error can't run : %s\n", cmd);
        strncpy(output, "N/A", size);
        output[size - 1] = '\0';
        return;
    }

    if (fgets(output, size, fp) != NULL) {
        output[strcspn(output, "\n")] = '\0';
    } else {
        strncpy(output, "N/A", size);
        output[size - 1] = '\0';
    }

    pclose(fp);
}

int getFetchInfo(char kernel[], char package[], char shell[], int *day,char hostname[]){
    char chBirth[SIZE_EXE];
    char chCurrent[SIZE_EXE];
    char chKernel[SIZE_EXE];

    run_command("pacman -Q | grep 'linux-hardened '", chKernel, SIZE_EXE);
    run_command("pacman -Q | wc -l", package, SIZE_EXE);
    run_command("pacman -Q | grep 'zsh '", shell, SIZE_EXE);
    run_command("stat -c %W /", chBirth, SIZE_EXE);
    run_command("date +%s", chCurrent, SIZE_EXE);

    cutAfterSpace(chKernel, kernel);

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

    getFetchInfo(kernel, package, shell, &day, hostname);


    // The constant is the number of char who never change (basicly number of char hard write on the printf)
    int numbKernel = 12 + strlen(kernel);
    int numbPackage = 12 + strlen(package);
    int numbShell = 12 + strlen(shell);
    int numbDay = 11;
    int numbHostname = 2 + strlen(hostname) + strlen(username);


    if (numOfCols >= (numbHostname + numbShell + numbDay + numbPackage)) printf(" \033[0;32m%s\033[0;31m@\033[0;34m%s \033[0;36m|\033[0;33m", username, hostname);
    if (numOfCols >= (numbKernel + numbDay + numbHostname + numbPackage + numbShell)) printf(" 󰣇\033[0;0m Linux %s \033[0;36m|\033[0;33m",kernel);
    if (numOfCols >= (numbPackage + numbShell)) printf(" 󰏖\033[0;0m %s (pacman) \033[0;36m|\033[0;33m",package);
    printf(" \033[0;0m %s ",shell);
    if(numOfCols >= (numbDay +numbShell + numbPackage))printf("\033[0;36m|\033[0;33m \033[0;0m  %d day",day);
    fflush(stdout);

    printf("\n");

    return 0;
}
