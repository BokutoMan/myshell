#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <utime.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>


#define HOME "~"
#define HISTORY_FILE ".command_history"


enum BashCommand {
    CD,
    LS,
    CP,
    MV,
    RM,
    PWD,
    PS,
    TREE,
    MKDIR,
    ECHO,
    CAT,
    TOUCH,
    // 添加其他常用命令...
    NUM_COMMANDS  // 用于统计命令的数量
};

// 将字符串转换为全大写
void convertToUpperCase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}




enum BashCommand selectCMD(char* commandStr){
   // 将字符串转换为全大写
    convertToUpperCase(commandStr);

    // 使用枚举值
    enum BashCommand myCommand;

    // 匹配枚举值
    if (strcmp(commandStr, "CD") == 0) {
        myCommand = CD;
    } else if (strcmp(commandStr, "LS") == 0) {
        myCommand = LS;
    } else if (strcmp(commandStr, "CP") == 0) {
        myCommand = CP;
    } else if (strcmp(commandStr, "MV") == 0) {
        myCommand = MV;
    } else if (strcmp(commandStr, "RM") == 0) {
        myCommand = RM;
    } else if (strcmp(commandStr, "PWD") == 0) {
        myCommand = PWD;
   } else if (strcmp(commandStr, "PS") == 0) {
        myCommand = PS;
    } else if (strcmp(commandStr, "TREE") == 0) {
        myCommand = TREE;
    } else if (strcmp(commandStr, "MKDIR") == 0) {
        myCommand = MKDIR;
    } else if (strcmp(commandStr, "ECHO") == 0) {
        myCommand = ECHO;
    } else if (strcmp(commandStr, "CAT") == 0) {
        myCommand = CAT;
    } else if (strcmp(commandStr, "TOUCH") == 0) {
        myCommand = TOUCH;
    } else {
        myCommand = NUM_COMMANDS;  // 默认值或表示未知命令
    }
  return myCommand;
}

void init_history() {
    using_history();
    read_history(HISTORY_FILE);
}

void save_history() {
    write_history(HISTORY_FILE);
}


void set_prompt(char* prompt) {
    snprintf(prompt, 290, "Your cmd>");
}

int do_cd(char*token){
    token = strtok(NULL, " ");
  // 尝试改变当前工作目录
    if (chdir(token) == 0) {
        printf("当前工作目录已更改为: %s\n", token);
    } else {
        perror("无法改变工作目录");
    }
}

int do_ls(char*token){
  // 打开当前目录或者
  DIR *dir=NULL;
  token = strtok(NULL, " ");
  if(!token)
    dir = opendir(".");
  else dir = opendir(token);

    if (dir == NULL) {
        perror("Unable to open directory");
        return 1;
    }

    // 读取目录内容
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        // 忽略隐藏文件（以点开头的文件）
        if (entry->d_name[0] != '.') {
            printf("%s\n", entry->d_name);
        }
    }

    // 关闭目录
    closedir(dir);
}

int do_cp(char* token){
    char source_path[100],dest_path[100];
    char command[256];
    token = strtok(NULL, " ");
    strcpy(source_path,token);
    token = strtok(NULL, " ");
    strcpy(dest_path,token);
    // 构建cp命令字符串
    snprintf(command, sizeof(command), "%s/cp %s %s", HOME, source_path, dest_path);

    // 调用系统命令
    int result = system(command);

    // 检查调用结果
    if (result == 0) {
        printf("Copy successful.\n");
    } else {
        fprintf(stderr, "Copy failed.\n");
    }
}

int do_mv(char* token){
    char source_path[100],dest_path[100];
    token = strtok(NULL, " ");
    strcpy(source_path,token);
    token = strtok(NULL, " ");
    strcpy(dest_path,token);

    // 调用系统命令
     int result = rename(source_path, dest_path);

    // 检查调用结果
    if (result == 0) {
        printf("Move successful.\n");
    } else {
        perror("Move failed");
        exit(EXIT_FAILURE);
    }
}

int do_rm(char* token){
    char dest_path[100];
    char command[256];
    token = strtok(NULL, " ");
    strcpy(dest_path,token);
    // 构建cp命令字符串
    snprintf(command, sizeof(command), "%s/rm %s", HOME, dest_path);

    // 调用系统命令
    int result = system(command);

    // 检查调用结果
    if (result == 0) {
        printf("Remove successful.\n");
    } else {
        perror("Remove failed");
        exit(EXIT_FAILURE);
    }
}
int do_pwd(){
  char current_path[4096]; 

    if (getcwd(current_path, sizeof(current_path)) != NULL) {
        // 获取当前工作目录成功，打印路径
        printf("%s\n", current_path);
    } else {
        perror("Error getting current working directory");
        exit(EXIT_FAILURE);
    }
}

int do_ps(){
    char command[256];
    snprintf(command, sizeof(command), "%s/ps", HOME);

    // 调用系统命令
    int result = system(command);

    // 检查调用结果
    if (result == 0) {
    } else {
        perror("PS failed");
        exit(EXIT_FAILURE);
    }
}

int do_tree(char* token){
    char dest_path[100];
    char command[256];
    int n = 0;
    token = strtok(NULL, " ");
    if(!token){
      snprintf(command, sizeof(command), "%s/tree", HOME);
    }
    else if(token != "-n"){
      strcpy(dest_path, token);
      token = strtok(NULL, " ");
    }
    else if (!token){
      snprintf(command, sizeof(command), "%s/tree %s", HOME, dest_path);
    }
    else{
    // 构建cp命令字符串
    snprintf(command, sizeof(command), "%s/tree %s -n %s", HOME, dest_path, token);
    }

    // 调用系统命令
    int result = system(command);

    // 检查调用结果
    if (result != 0) {
        perror("Tree failed");
        exit(EXIT_FAILURE);
    }
}

int do_mkdir(char*token){
    token = strtok(NULL, " ");
  // 使用mkdir函数创建目录
    if (mkdir(token, 0777) == 0) {
        printf("Directory '%s' created successfully.\n", token);
    } else {
        perror("Error creating directory");
        exit(EXIT_FAILURE);
    }
}

int do_echo(char*token){
    token = strtok(NULL, " ");
    while(token!= NULL){
      printf("%s",token);
      printf(" ");
      token = strtok(NULL, " ");
    }
    printf("\n");
}

int do_cat(char*token){
    char filename[512];
    token = strtok(NULL, " ");
    strcpy(filename, token);
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char ch;

    // 逐字符读取文件内容并输出到标准输出
    while ((ch = fgetc(file)) != EOF) {
        putchar(ch);
    }

    fclose(file);
}

int do_touch(char*token){
    char filename[512];
    token = strtok(NULL, " ");
    strcpy(filename, token);
    struct stat st;

    // 检查文件是否存在
    if (stat(filename, &st) != 0) {
        // 文件不存在，创建一个空文件
        FILE *file = fopen(filename, "w");
        if (file == NULL) {
            perror("Error creating file");
            return 1;
        }
        fclose(file);
    } else {
        // 文件存在，更新访问和修改时间
        struct utimbuf new_times;
        new_times.actime = st.st_atime;
        new_times.modtime = time(NULL);
        
        if (utime(filename, &new_times) != 0) {
            perror("Error updating file times");
            return 1;
        }
    }
}

void do_execute_command(char *token){
      char mycommand[30];
    // 使用strtok分割字符串
      token = strtok(token, " ");
      strcpy(mycommand, token);
      enum BashCommand myCommand = selectCMD(mycommand);
      
       // 使用枚举值
    switch (myCommand) {
        case CD:
            do_cd(token);
            break;
        case LS:
            do_ls(token);
            break;
        case CP:
            do_cp(token);
            break;
        case MV:
            do_mv(token);
            break;
        case RM:
            do_rm(token);
            break;
        case PWD:
            do_pwd();
            break;
        case PS:
            do_ps();
            break;
        case TREE:
            do_tree(token);
            break;
        case MKDIR:
            do_mkdir(token);
            break;
        case ECHO:
            do_echo(token);
            break;
        case CAT:
            do_cat(token);
            break;
        case TOUCH:
            do_touch(token);
            break;
        // 添加其他命令的处理...

        default:
            printf("Unknown command\n");
            break;
    }
  
}

int areFileDescriptorsEqual(int fd1, int fd2) {
    struct stat stat1, stat2;

    // 获取文件1的信息
    if (fstat(fd1, &stat1) == -1) {
        return -1; // 获取信息失败
    }

    // 获取文件2的信息
    if (fstat(fd2, &stat2) == -1) {
        return -1; // 获取信息失败
    }

    // 判断两个文件描述符是否指向同一个文件
    return (stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino);
}

void execute_command(char *command, int in_fd, int out_fd, int is_background) {
//     printf("Command: %s\n", command);
//     printf("Input File Descriptor: %d\n", in_fd);
//     printf("Output File Descriptor: %d\n", out_fd);
//     printf("Is Background: %d\n", is_background);
        
          // 保存原始标准输入和标准输出
        int saved_stdin = dup(STDIN_FILENO);
        int saved_stdout = dup(STDOUT_FILENO);


        int re_in_fd = areFileDescriptorsEqual(in_fd ,STDIN_FILENO);
        int re_out_fd = areFileDescriptorsEqual(out_fd, STDOUT_FILENO);
  
//         printf("\n\n  re_in_fd  %d \n re_out_fd  %d \n\n",re_in_fd,re_out_fd);
        if(re_in_fd == 0){
          dup2(in_fd, STDIN_FILENO);
           close(in_fd);
        }
        
        if(re_out_fd == 0){
          dup2(out_fd, STDOUT_FILENO);
          close(out_fd);
        }

        if(!is_background){    //没有  &  操作符
          do_execute_command(command);

        }
        else{   //静默进程，由子进程执行
          pid_t pid = fork(); // 创建子进程

          if (pid == -1) {
              perror("fork failed");
              exit(EXIT_FAILURE);
          }

          if (pid == 0) {
              // 在子进程中
              do_execute_command(command);
              exit(EXIT_SUCCESS); // 子进程执行完毕后退出
          }
        }
//         printf("saved_stdin: %d\n",saved_stdin);
//         printf("saved_stdout: %d\n",saved_stdout);

      // 还原标准输入和标准输出
      dup2(saved_stdin, STDIN_FILENO);
      dup2(saved_stdout, STDOUT_FILENO);

      // 关闭保存的文件描述符
      close(saved_stdin);
      close(saved_stdout);

}

int main() {
    char prompt[300];
    char mycommand[30];
    char* line;
    char *token;
    char *rest;
    char input_[1024];
    char input[1024];
    // 初始化历史记录
    init_history();
  
    while (1) {
      set_prompt(prompt);
      if (!(line = readline(prompt)))
          break;
      strcpy(input_, line);
      strcpy(input, input_);
      add_history(input);
//       printf("%s",input);
//       token = strtok_r(input, ">", &rest);
//       printf("input: %s\n token:%s \n rest:%s\n",input,token,rest);
      
      
      int in_fd = STDIN_FILENO;

      int is_background = 0;
      if (strstr(input, "&")) {
            is_background = 1;
            input[strcspn(input, "&")] = '\0'; // Remove '&'
        }
      token = strtok_r(input, "|", &rest);
      
      if (rest == NULL) {
            int pipe_fds[2];
            pipe(pipe_fds);

            int out_fd = pipe_fds[1];

            while (token != NULL) {
                execute_command(token, in_fd, out_fd, is_background);

                // 移动到下一个命令
                in_fd = pipe_fds[0];
                token = strtok_r(NULL, "|", &rest);
                if (token != NULL) {
                    close(pipe_fds[1]);
                    pipe(pipe_fds);
                    out_fd = pipe_fds[1];
                }
            }

            close(pipe_fds[0]);
            close(pipe_fds[1]);
        } else { // 没有管道
            // 查看有没有重定向
            int out_fd = STDOUT_FILENO;
        
//             printf("out_fd  %d",out_fd);
        
            strcpy(input, input_);
            token = strtok_r(input, ">", &rest);
//         printf("输出重定向 \n input: %s\n token:%s \n rest:%s\n",input,token,rest);
            if (rest != NULL) {
                // 输出重定向
                char *output_file = strtok_r(NULL, ">", &rest);
                if (output_file != NULL) {
                    out_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
            }
            strcpy(input, input_);
            token = strtok_r(input, "<", &rest);
//         printf("输入重定向 \n input: %s\n token:%s \n rest:%s\n",input,token,rest);
            if (rest != NULL) {
                // 输入重定向
                char *input_file = strtok_r(NULL, "<", &rest);
                if (input_file != NULL) {
                    in_fd = open(input_file, O_RDONLY);
                }
            }

            execute_command(token, in_fd, out_fd, is_background);

            if (in_fd != STDIN_FILENO) {
                close(in_fd);
            }

            if (out_fd != STDOUT_FILENO) {
                close(out_fd);
            }
        }
      

        
      // 保存历史记录
      save_history();
      free(line);
    }

    return 0;
}
