#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

void print_tree(const char *path, int max_depth, int current_depth) {
    if (current_depth > max_depth && max_depth != -1) {
        return;
    }

    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            for (int i = 0; i < current_depth; i++) {
                printf("  ");
            }

            printf("|-- %s\n", entry->d_name);

            char new_path[1024];
            snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);

            struct stat statbuf;
            if (stat(new_path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
                print_tree(new_path, max_depth, current_depth + 1);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    const char *path = (argc == 2) ? argv[1] : ".";  // 默认为当前目录
    int max_depth = -1;  // 默认显示所有层级

    if (argc == 4 && strcmp(argv[2], "-n") == 0) {
        max_depth = atoi(argv[3]);
    }

    printf("%s\n", path);
    print_tree(path, max_depth, 0);

    return 0;
}
