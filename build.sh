#!/bin/bash

# 获取当前目录下的所有.c文件
c_files=$(ls *.c)

# 编译每个.c文件
for file in $c_files
do
  # main.c文件单独处理
  if [[ "$file" == "main.c" ]]; then
    gcc main.c -o myshell -lreadline
    # 检查编译是否成功
    if [ $? -eq 0 ]; then
      echo "Compiled $file successfully."
    else
      echo "Failed to compile $file."
      exit 1
    fi
    continue
  fi
  
  if [[ "$file" == "redx.c" ]]; then
    continue
  fi

  # 提取文件名（去掉扩展名）
  file_name="${file%.*}"

  # 编译
  gcc -o $file_name $file

  # 检查编译是否成功
  if [ $? -eq 0 ]; then
    echo "Compiled $file successfully."
  else
    echo "Failed to compile $file."
    exit 1
  fi
done

# 提示编译完成
echo "Compilation complete."
