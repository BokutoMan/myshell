#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

void list_processes() {
    printf("%-5s %-10s %-15s %s\n", "PID", "TTY", "TIME", "CMD");

    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (isdigit(entry->d_name[0])) {
            char proc_path[PATH_MAX];
            snprintf(proc_path, sizeof(proc_path), "/proc/%s", entry->d_name);

            struct stat statbuf;
            if (stat(proc_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                char stat_path[PATH_MAX];
                snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

                FILE *stat_file = fopen(stat_path, "r");
                if (stat_file == NULL) {
                    perror("fopen");
                    continue;
                }

                int pid;
                char comm[256];
                char state;
                int ppid, pgrp, session, tty_nr, tpgid;
                unsigned int flags;
                unsigned long minflt, cminflt, majflt, cmajflt;
                unsigned long utime, stime;

                fscanf(stat_file, "%d %s %c %d %d %d %d %d %u "
                                  "%lu %lu %lu %lu %lu %lu",
                       &pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid,
                       &flags, &minflt, &cminflt, &majflt, &cmajflt, &utime, &stime);

                fclose(stat_file);

                char tty[32];
                snprintf(tty, sizeof(tty), "pts/%d", tty_nr);

                unsigned long total_time = utime + stime;
                unsigned long seconds = total_time / sysconf(_SC_CLK_TCK);
                unsigned long minutes = seconds / 60;
                unsigned long hours = minutes / 60;

                printf("%-5d %-10s %02lu:%02lu:%02lu %s\n", pid, tty, hours, minutes % 60, seconds % 60, comm);
            }
        }
    }

    closedir(dir);
}

int main() {
    list_processes();
    return 0;
}
