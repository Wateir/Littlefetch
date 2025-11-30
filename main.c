#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <alpm.h>
#include <libgen.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <asm-generic/ioctls.h>
#include <sys/sysinfo.h>

#define SIZE_EXE 256
#define SEPARATOR "\033[0;36m|\033[0;33m "

typedef struct {
    int cost;
    int mandatory;
    void (*print_fn)(void);
} Segment;

static char g_username[SIZE_EXE];
static char g_hostname[SIZE_EXE];
static char g_kernel[SIZE_EXE];
static char g_package[SIZE_EXE];
static char g_shell[SIZE_EXE];
static char g_day[SIZE_EXE];
static char g_uptime[SIZE_EXE];

int giveSizeTerminal() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

int getHostname(char hostname[]) {
    if (gethostname(hostname, SIZE_EXE) != 0) {
        perror("gethostname failed");
        return 1;
    }
    return 0;
}

time_t getUptime() {
    struct sysinfo s;
    if (sysinfo(&s) != 0) return (time_t)-1;
    return (time_t)s.uptime;
}

time_t getAge() {
    time_t current, birth;
    struct statx stats;
    statx(AT_FDCWD, "/", AT_SYMLINK_NOFOLLOW, STATX_BTIME, &stats);
    current = time(NULL);
    birth = (time_t) stats.stx_btime.tv_sec;
    return difftime(current, birth) / 86400;
}

void formatUptime(time_t sec, char *buf, size_t size) {
    long days    = sec / 86400;
    long hours   = (sec % 86400) / 3600;
    long minutes = (sec % 3600) / 60;

    if (days > 0)
        snprintf(buf, size, "%ldd%02ldh%02ldm", days, hours, minutes);
    else
        snprintf(buf, size, "%ldh%02ldm", hours, minutes);
}

void formatDay(time_t day_sec, char *buf, size_t size) {
    snprintf(buf, size, "%ld", day_sec);
}

int getFetchInfo(char kernel[], char package[], char shell[],
                 char day[], char hostname[], char uptime[]) {

    alpm_handle_t *handle;
    alpm_db_t *localdb;
    alpm_pkg_t *pkg;
    alpm_list_t *pkgcache;
    struct utsname buffer;
    const char *shell_path = getenv("SHELL");

    handle = alpm_initialize("/", "/var/lib/pacman", NULL);
    localdb = alpm_get_localdb(handle);
    pkgcache = alpm_db_get_pkgcache(localdb);

    snprintf(package, SIZE_EXE, "%d", (int)alpm_list_count(pkgcache));

    uname(&buffer);
    strcat(kernel, buffer.release);

    char *shell_copy = strdup(shell_path);
    pkg = alpm_db_get_pkg(localdb, basename(shell_copy));
    snprintf(shell, SIZE_EXE, "%s %s", alpm_pkg_get_name(pkg), alpm_pkg_get_version(pkg));
    free(shell_copy);

    getHostname(hostname);

    time_t days_sec = getAge();
    formatDay(days_sec, day, SIZE_EXE);

    time_t uptime_sec = getUptime();
    formatUptime(uptime_sec, uptime, SIZE_EXE);

    return 0;
}

void print_userhost(void) { printf(" \033[0;32m%s\033[0;31m@\033[0;34m%s ", g_username, g_hostname); }
void print_kernel(void)   { printf(" 󰣇\033[0;0m Linux %s ", g_kernel); }
void print_packages(void) { printf(" 󰏖\033[0;0m %s (pacman) ", g_package); }
void print_shell(void)    { printf(" \033[0;0m %s ", g_shell); }
void print_day(void)      { printf(" \033[0;0m %s day ", g_day); }
void print_uptime(void)   { printf(" \033[0;0m %s ", g_uptime); }

int main(void) {
    int numOfCols = giveSizeTerminal();

    char kernel[SIZE_EXE] = {0};
    char package[SIZE_EXE] = {0};
    char shell[SIZE_EXE] = {0};
    char hostname[SIZE_EXE] = {0};
    char day[SIZE_EXE] = {0};
    char uptime[SIZE_EXE] = {0};

    char *username_env = getenv("USER");
    if (!username_env) username_env = "user";
    snprintf(g_username, SIZE_EXE, "%s", username_env);

    getFetchInfo(kernel, package, shell, day, hostname, uptime);

    snprintf(g_hostname, SIZE_EXE, "%s", hostname);
    snprintf(g_kernel, SIZE_EXE, "%s", kernel);
    snprintf(g_package, SIZE_EXE, "%s", package);
    snprintf(g_shell, SIZE_EXE, "%s", shell);
    snprintf(g_day, SIZE_EXE, "%s", day);
    snprintf(g_uptime, SIZE_EXE, "%s", uptime);

    int numbKernel   = 12 + strlen(g_kernel);
    int numbPackage  = 12 + strlen(g_package);
    int numbShell    = 12 + strlen(g_shell);
    int numbDay      = 12;
    int numbHostname = 2 + strlen(g_hostname) + strlen(g_username);
    int numbUptime   = 12;

    Segment segments[] = {
        { numbHostname, 0, print_userhost },
        { numbKernel,   0, print_kernel },
        { numbPackage,  0, print_packages },
        { numbShell,    1, print_shell },
        { numbDay,      0, print_day },
        { numbUptime,   0, print_uptime }
    };

    int segcount = sizeof(segments) / sizeof(Segment);
    int used = 0;
    int printed_any = 0;

    for (int i = 0; i < segcount; i++) {
        if (!segments[i].mandatory && numOfCols < used + segments[i].cost)
            continue;
        if (printed_any) {
            printf(SEPARATOR);
            used += 3;
        }
        segments[i].print_fn();
        used += segments[i].cost;
        printed_any = 1;
    }

    printf("\n");
    return 0;
}
