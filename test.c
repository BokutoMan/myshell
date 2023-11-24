#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 去掉字符串两端的空格和回车
char* trim(char* str) {
    int len = strlen(str);

    // 去掉开头的空格和回车
    int start = 0;
    while (start < len && (isspace((unsigned char)str[start]) || str[start] == '\n')) {
        start++;
    }

    // 去掉末尾的空格和回车
    int end = len - 1;
    while (end >= 0 && (isspace((unsigned char)str[end]) || str[end] == '\n')) {
        end--;
    }

    // 如果字符串全是空格和回车，则直接置为空字符串
    if (start > end) {
        str[0] = '\0';
    } else {
        // 移动有效的部分到字符串的开头
        memmove(str, str + start, end - start + 1);
        str[end - start + 1] = '\0'; // 在有效部分的末尾添加字符串结束符
    }

    return str;
}

int main() {
    char input[] = "  \t  Hello, World!  \n  ";

    printf("Original: [%s]\n", input);

    trim(input);

    printf("Trimmed: [%s]\n", input);

    return 0;
}
