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

int getHostname(char hostname[]){

    if (gethostname(hostname, SIZE_EXE) != 0) {
        perror("gethostname failed");
        return 1;
    }
    return 0;
}

int main(void){
    alpm_handle_t *handle;
    alpm_db_t *localdb;
    alpm_pkg_t *pkg;
    alpm_list_t *pkgcache;
    const char *shell_path = getenv("SHELL");
    time_t current, birth, diff;
    char hostname[SIZE_EXE];
    struct statx stats;

    char *shell_copy = strdup(shell_path);
    printf("Shell name: %s\n", basename(shell_copy));

    handle = alpm_initialize("/", "/var/lib/pacman", NULL);
    localdb = alpm_get_localdb(handle);
    pkg = alpm_db_get_pkg(localdb, basename(shell_copy));

    if (pkg) {
        printf("Version: %s\n", alpm_pkg_get_version(pkg));
    }

    pkgcache = alpm_db_get_pkgcache(localdb);
    int count = alpm_list_count(pkgcache);

       printf("Nombre de paquets installés : %d\n", count);
    alpm_release(handle);


    struct utsname buffer;

    if (uname(&buffer) != 0) {
        perror("uname");
        return 1;
    }

    printf("Kernel name: %s\n", buffer.release);

    free(shell_copy);

    statx(AT_FDCWD, "/", AT_SYMLINK_NOFOLLOW, STATX_BTIME, &stats);

    current = time(NULL);
    birth = (time_t) stats.stx_btime.tv_sec;
    diff = difftime(current, birth) / 86400;

    getHostname(hostname);
    char* username = getenv("USER");

    printf("Age: %ld\n", diff);
    printf("Age: %s@%s\n",username, hostname);
    return 0;
}
