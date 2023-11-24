#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>


void remove_file(const char *path);
void remove_directory(const char *path);

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 获取路径
    const char *path = argv[1];

    // 获取文件/目录信息
    struct stat path_info;
    if (stat(path, &path_info) == -1) {
        perror("Error getting file/directory information");
        exit(EXIT_FAILURE);
    }

    // 如果是文件，则直接删除
    if (S_ISREG(path_info.st_mode)) {
        remove_file(path);
    }
    // 如果是目录，则递归删除
    else if (S_ISDIR(path_info.st_mode)) {
        remove_directory(path);
    }
    // 如果既不是文件也不是目录，则报错
    else {
        fprintf(stderr, "Unsupported type\n");
        exit(EXIT_FAILURE);
    }


    return 0;
}

// 删除文件
void remove_file(const char *path) {
    int result = unlink(path);
    if (result == -1) {
        perror("Error removing file");
        exit(EXIT_FAILURE);
    }
}

// 递归删除目录
void remove_directory(const char *path) {
    DIR *dir;
    struct dirent *entry;

    // 打开目录
    dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // 遍历目录中的文件和子目录，并递归删除它们
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // 构建文件/目录的路径
            char entry_path[PATH_MAX];
            snprintf(entry_path, PATH_MAX, "%s/%s", path, entry->d_name);

            // 获取文件/目录的信息
            struct stat entry_info;
            if (stat(entry_path, &entry_info) == -1) {
                perror("Error getting file/directory information");
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            // 如果是文件，则直接删除
            if (S_ISREG(entry_info.st_mode)) {
                remove_file(entry_path);
            }
            // 如果是目录，则递归删除
            else if (S_ISDIR(entry_info.st_mode)) {
                remove_directory(entry_path);
            }
        }
    }

    // 关闭目录流
    closedir(dir);

    // 删除空目录
    int result = rmdir(path);
    if (result == -1) {
        perror("Error removing directory");
        exit(EXIT_FAILURE);
    }
}
