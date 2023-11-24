#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

#define BUFFER_SIZE 4096

// 函数声明
void copy_file(const char *source, const char *destination);
void copy_directory(const char *source, const char *destination);

int main(int argc, char *argv[]) {
    // 检查参数数量
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 获取源文件/目录和目标文件/目录路径
    const char *source = argv[1];
    const char *destination = argv[2];

    // 获取源文件/目录的信息
    struct stat source_info;
    if (stat(source, &source_info) == -1) {
        perror("Error getting source information");
        exit(EXIT_FAILURE);
    }

    // 如果源是文件，则直接复制文件
    if (S_ISREG(source_info.st_mode)) {
        copy_file(source, destination);
    }
    // 如果源是目录，则递归复制目录
    else if (S_ISDIR(source_info.st_mode)) {
        copy_directory(source, destination);
    }
    // 如果源既不是文件也不是目录，则报错
    else {
        fprintf(stderr, "Unsupported source type\n");
        exit(EXIT_FAILURE);
    }

    printf("Copy completed successfully.\n");
    return 0;
}

// 复制文件
void copy_file(const char *source, const char *destination) {
    int source_fd, dest_fd;
    ssize_t bytes_read, bytes_written;
    char buffer[BUFFER_SIZE];

    // 打开源文件
    source_fd = open(source, O_RDONLY);
    if (source_fd == -1) {
        perror("Error opening source file");
        exit(EXIT_FAILURE);
    }

    // 创建目标文件（如果不存在则创建，存在则截断为空）
    dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(source_fd);
        exit(EXIT_FAILURE);
    }

    // 从源文件读取数据并写入目标文件
    while ((bytes_read = read(source_fd, buffer, sizeof(buffer))) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(source_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }

    // 关闭文件描述符
    close(source_fd);
    close(dest_fd);
}

// 复制目录
void copy_directory(const char *source, const char *destination) {
    DIR *dir;
    struct dirent *entry;

    // 打开源目录
    dir = opendir(source);
    if (dir == NULL) {
        perror("Error opening source directory");
        exit(EXIT_FAILURE);
    }

    // 创建目标目录
    if (mkdir(destination, 0777) == -1) {
        perror("Error creating destination directory");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    // 遍历源目录中的文件和子目录，并递归复制它们
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            // 构建源文件/目录和目标文件/目录的路径
            char source_path[PATH_MAX];
            char dest_path[PATH_MAX];
            snprintf(source_path, PATH_MAX, "%s/%s", source, entry->d_name);
            snprintf(dest_path, PATH_MAX, "%s/%s", destination, entry->d_name);

            // 获取源文件/目录的信息
            struct stat entry_info;
            if (stat(source_path, &entry_info) == -1) {
                perror("Error getting source information");
                closedir(dir);
                exit(EXIT_FAILURE);
            }

            // 如果是文件，则直接复制
            if (S_ISREG(entry_info.st_mode)) {
                copy_file(source_path, dest_path);
            }
            // 如果是目录，则递归复制
            else if (S_ISDIR(entry_info.st_mode)) {
                copy_directory(source_path, dest_path);
            }
        }
    }

    // 关闭目录流
    closedir(dir);
}
