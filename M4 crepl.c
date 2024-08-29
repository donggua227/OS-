#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_CODE_FILE "/tmp/crepl_code.c"
#define TEMP_BIN_FILE "/tmp/crepl_bin"

// 初始化代码文件，写入头文件
void init_code_file() {
    FILE *fp = fopen(TEMP_CODE_FILE, "w");
    if (!fp) {
        perror("fopen");
        return;
    }
    fprintf(fp, "#include <stdio.h>\n");
    fclose(fp);
}

// 追加函数定义到代码文件中
void append_to_code_file(const char *code) {
    FILE *fp = fopen(TEMP_CODE_FILE, "a");
    if (!fp) {
        perror("fopen");
        return;
    }
    fprintf(fp, "%s\n", code);
    fclose(fp);
}

// 编译代码并生成可执行文件
int compile_code() {
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "gcc -o %s %s", TEMP_BIN_FILE, TEMP_CODE_FILE);
    return system(cmd);
}

// 运行编译后的可执行文件
void run_expression() {
    system(TEMP_BIN_FILE);
}

// 处理表达式，将其插入到 main 函数中的 printf 语句
void handle_expression(const char *expr) {
    append_to_code_file("int main() {");
    char wrapped_expr[4096];
    snprintf(wrapped_expr, sizeof(wrapped_expr), "printf(\"%%d\\n\", %s);", expr);
    append_to_code_file(wrapped_expr);
    append_to_code_file("return 0; }");
    if (compile_code() == 0) {
        run_expression();
    } else {
        printf("Error compiling expression.\n");
    }
    // 重新初始化代码文件，避免重复 main 函数
    init_code_file();
}

int main() {
    char line[4096];
    
    init_code_file();

    while (1) {
        printf("crepl> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }

        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "int ", 4) == 0) {
            // 如果是函数定义，追加到代码文件
            append_to_code_file(line);
            printf("Function definition added.\n");
        } else {
            // 如果是表达式，处理表达式
            handle_expression(line);
        }
    }

    return 0;
}
