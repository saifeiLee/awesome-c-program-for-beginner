# Tiny macro processor

该程序旨在实现一个宏预处理器，作用是将宏定义转换成对应的字符串值，例如:

```
#define SIZE 100

int main() {
    int a = SIZE;
}
```

转换为：

```
#define SIZE 100

int main() {
    int a = 100;
}
```

实现该程序有几个关键点：
1. 如何读一个单词块
2. 如何管理已定义的宏
3. 如何处理特殊内容，例如注释、单引号对、双引号对