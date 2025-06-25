#define _GNU_SOURCE
#define SIZE_EXE 256

#include <stdio.h>
#include <string.h>
#include <alpm.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctls.h>
#include <linux/stat.h>

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

time_t getUptime(){
    time_t current, birth, diff;
    struct statx stats;

    statx(AT_FDCWD, "/", AT_SYMLINK_NOFOLLOW, STATX_BTIME, &stats);

    current = time(NULL);
    birth = (time_t) stats.stx_btime.tv_sec;
    diff = difftime(current, birth) / 86400;

    return diff;
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

int getFetchInfo(char kernel[], char package[], char shell[], time_t *day,char hostname[]){
    char chBirth[SIZE_EXE];
    char chCurrent[SIZE_EXE];
    char chKernel[SIZE_EXE];

    run_command("pacman -Q | grep 'linux-hardened '", chKernel, SIZE_EXE);
    run_command("pacman -Q | wc -l", package, SIZE_EXE);
    run_command("pacman -Q | grep 'zsh '", shell, SIZE_EXE);
    run_command("stat -c %W /", chBirth, SIZE_EXE);
    run_command("date +%s", chCurrent, SIZE_EXE);

    cutAfterSpace(chKernel, kernel);

    *day = getUptime();

    getHostname(hostname);

    return 0;
}

int main(void){
    int numOfCols = giveSizeTerminal();
    char kernel[SIZE_EXE];
    char package[SIZE_EXE];
    char shell[SIZE_EXE];
    char hostname[SIZE_EXE];
    char* username = getenv("USER");
    time_t day;

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
    if(numOfCols >= (numbDay +numbShell + numbPackage))printf("\033[0;36m|\033[0;33m \033[0;0m  %ld day",day);
    fflush(stdout);

    printf("\n");


    alpm_handle_t *handle;
    alpm_db_t *localdb;
    alpm_pkg_t *pkg;

    handle = alpm_initialize("/", "/var/lib/pacman", NULL);
    localdb = alpm_get_localdb(handle);
    pkg = alpm_db_get_pkg(localdb, "linux");

    if (pkg) {
        printf("Version: %s\n", alpm_pkg_get_version(pkg));
    }

    alpm_release(handle);

    return 0;
}
