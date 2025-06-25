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

int getFetchInfo(char kernel[], char package[], char shell[], time_t *day,char hostname[]){
    alpm_handle_t *handle;
    alpm_db_t *localdb;
    alpm_pkg_t *pkg;
    alpm_list_t *pkgcache;
    struct utsname buffer;
    const char *shell_path = getenv("SHELL");

    handle = alpm_initialize("/", "/var/lib/pacman", NULL);
    localdb = alpm_get_localdb(handle);
    pkgcache = alpm_db_get_pkgcache(localdb);

    sprintf(package,"%d",(int)alpm_list_count(pkgcache));
    uname(&buffer);
    strcat(kernel,buffer.release);

    char *shell_copy = strdup(shell_path);
    pkg = alpm_db_get_pkg(localdb, basename(shell_copy));
    strcat(shell,(char *) alpm_pkg_get_name(pkg));
    strcat(shell," ");
    strcat(shell,(char *) alpm_pkg_get_version(pkg));

    *day = getUptime();
    getHostname(hostname);

    free(shell_copy);
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
    return 0;
}
