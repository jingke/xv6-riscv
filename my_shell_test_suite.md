# my_shell 测试套件

本测试套件旨在全面验证my_shell的各项功能，包括基本命令执行、输入/输出重定向、管道、序列命令以及各种边界条件。

## 目录
1. [基本功能测试](#基本功能测试)
2. [输入/输出重定向测试](#输入输出重定向测试)
3. [管道测试](#管道测试)
4. [序列命令测试](#序列命令测试)
5. [边界条件测试](#边界条件测试)
6. [错误处理测试](#错误处理测试)
7. [综合测试](#综合测试)

## 基本功能测试

### 1. 简单命令执行
```bash
ls
```
预期结果：列出当前目录内容

### 2. 带参数的命令执行
```bash
ls -l
```
预期结果：以长格式列出当前目录内容

### 3. 带多个参数的命令执行
```bash
echo hello world
```
预期结果：输出"hello world"

### 4. 内置命令 cd
```bash
cd /
ls
cd tmp
ls
```
预期结果：能够切换目录并显示相应目录的内容

### 5. 内置命令 exit
```bash
exit
```
预期结果：shell退出

## 输入/输出重定向测试

### 1. 输入重定向
首先创建测试文件：
```bash
echo "Hello, World!" > test_input.txt
```
然后进行输入重定向测试：
```bash
wc -c < test_input.txt
```
预期结果：显示文件字符数（应该是14，包括换行符）

### 2. 输出重定向
```bash
echo "Test output redirection" > test_output.txt
cat test_output.txt
```
预期结果：文件test_output.txt应该包含"Test output redirection"

### 3. 追加输出重定向
```bash
echo "First line" > test_append.txt
echo "Second line" >> test_append.txt
cat test_append.txt
```
预期结果：文件应包含两行内容

### 4. 同时使用输入和输出重定向
```bash
echo "Input for cat" > input.txt
cat < input.txt > output.txt
cat output.txt
```
预期结果：output.txt应该包含"Input for cat"

## 管道测试

### 1. 简单管道
```bash
ls | wc -l
```
预期结果：显示当前目录中的文件数量

### 2. 多级管道
```bash
ls | grep txt | wc -l
```
预期结果：显示当前目录中.txt文件的数量

### 3. 管道与重定向结合
```bash
ls | grep txt > txt_files.txt
cat txt_files.txt
```
预期结果：txt_files.txt应该包含所有.txt文件的列表

## 序列命令测试

### 1. 简单序列命令
```bash
pwd ; ls
```
预期结果：先显示当前目录路径，然后列出目录内容

### 2. 多个序列命令
```bash
pwd ; ls ; echo "Done"
```
预期结果：依次执行三个命令

### 3. 序列命令与重定向结合
```bash
echo "First" > seq_test.txt ; echo "Second" >> seq_test.txt ; cat seq_test.txt
```
预期结果：seq_test.txt应该包含两行内容

### 4. 序列命令与管道结合
```bash
ls > files.txt ; cat files.txt | wc -l
```
预期结果：显示files.txt中的行数（等于当前目录中的文件数）

## 边界条件测试

### 1. 空命令
```bash
[按回车键]
```
预期结果：显示新的提示符，不执行任何操作

### 2. 只有空格的命令
```bash
    [按回车键]
```
预期结果：显示新的提示符，不执行任何操作

### 3. 最大参数数量测试
```bash
echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9
```
预期结果：成功执行（最多支持10个参数，包括命令本身）

### 4. 超过最大参数数量测试
```bash
echo arg1 arg2 arg3 arg4 arg5 arg6 arg7 arg8 arg9 arg10
```
预期结果：应该显示错误信息"Error: Too many arguments (maximum 10)"

### 5. 长命令行测试
```bash
echo "这是一个很长的命令行测试，用来验证my_shell能否正确处理长输入"
```
预期结果：正确输出该字符串

### 6. 特殊字符处理
```bash
echo "Special chars: !@#$%^&*()"
```
预期结果：正确输出特殊字符

## 错误处理测试

### 1. 不存在的命令
```bash
nonexistentcommand
```
预期结果：显示错误信息"Error executing command: nonexistentcommand"

### 2. 不存在的输入文件
```bash
cat < nonexistent.txt
```
预期结果：显示错误信息"Error opening input file: nonexistent.txt"

### 3. 无法创建的输出文件（权限问题）
```bash
echo "test" > /root/test.txt
```
预期结果：显示错误信息"Error opening output file: /root/test.txt"

### 4. 无效的目录切换
```bash
cd /nonexistentdirectory
```
预期结果：显示错误信息"Error changing directory: /nonexistentdirectory"

### 5. 管道创建失败模拟
这个测试较难模拟，但可以通过代码审查确认错误处理逻辑

## 综合测试

### 1. 复杂组合命令
```bash
ls | grep txt > txt_files.txt ; cat txt_files.txt | wc -l ; echo "Found $(cat txt_files.txt | wc -l) txt files" > result.txt ; cat result.txt
```
预期结果：正确执行所有步骤并产生预期输出

### 2. 嵌套结构测试
```bash
ls | grep txt | wc -l > count.txt ; echo "Count: $(cat count.txt)" ; rm count.txt
```
预期结果：正确计算.txt文件数量并显示，最后删除临时文件

### 3. 多层管道和重定向
```bash
cat /dev/random | head -c 100 | od -tx1 -Ad > random_hex.txt ; cat random_hex.txt | wc -l
```
预期结果：生成随机十六进制数据并统计行数

## 自动化测试脚本

为了方便自动化测试，可以创建以下测试脚本：

### test_basic.sh
```bash
#!/bin/sh
echo "Testing basic commands..."
ls
echo hello world
pwd
echo "Basic tests completed."
```

### test_redirection.sh
```bash
#!/bin/sh
echo "Testing redirection..."
echo "Hello, World!" > test_input.txt
wc -c < test_input.txt
echo "Test output redirection" > test_output.txt
cat test_output.txt
echo "Redirection tests completed."
```

### test_pipes.sh
```bash
#!/bin/sh
echo "Testing pipes..."
ls | wc -l
ls | grep txt | wc -l
echo "Pipe tests completed."
```

### test_sequences.sh
```bash
#!/bin/sh
echo "Testing sequences..."
pwd ; ls
echo "First" > seq_test.txt ; echo "Second" >> seq_test.txt ; cat seq_test.txt
echo "Sequence tests completed."
```

## 性能和压力测试

### 1. 大量命令连续执行
```bash
i=0
while [ $i -lt 100 ]
do
  echo "Command number $i"
  i=`expr $i + 1`
done
```
预期结果：能够顺利执行100次循环

### 2. 快速连续命令
快速连续输入多个命令，验证shell的稳定性

## 注意事项

1. 所有测试应在干净的环境中进行，避免现有文件干扰测试结果
2. 测试完成后应清理产生的临时文件
3. 某些测试可能需要特定的文件权限设置
4. 测试过程中应注意观察内存使用情况，防止内存泄漏

## 测试结果记录

建议在测试过程中记录以下信息：
- 测试用例编号
- 实际输出
- 预期输出
- 测试通过/失败状态
- 发现的问题和错误信息

通过以上全面的测试套件，可以有效验证my_shell的各种功能和边界条件处理能力。