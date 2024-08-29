#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 16

// 辅助函数，获取两个整数中的最大值
int max(int a, int b) {
    return (a > b) ? a : b;
}

// 线程参数结构体
typedef struct {
    int i;
    int j;
    char *X;
    char *Y;
    int **L;
    pthread_mutex_t *mutex;
} ThreadData;

void *lcs_thread(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int i = data->i;
    int j = data->j;
    
    // 计算当前单元格的值
    pthread_mutex_lock(data->mutex);  // 加锁，保护共享资源
    if (data->X[i - 1] == data->Y[j - 1])
        data->L[i][j] = data->L[i - 1][j - 1] + 1;
    else
        data->L[i][j] = max(data->L[i - 1][j], data->L[i][j - 1]);
    pthread_mutex_unlock(data->mutex);  // 解锁

    pthread_exit(NULL);
}

// LCS算法的主函数，使用并行波前计算
int lcs(char *X, char *Y, int m, int n, int T) {
    int i, j, round;
    pthread_t threads[MAX_THREADS];
    ThreadData thread_data[MAX_THREADS];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    int **L = (int **)malloc((m + 1) * sizeof(int *));
    for (i = 0; i <= m; i++) {
        L[i] = (int *)malloc((n + 1) * sizeof(int));
        memset(L[i], 0, (n + 1) * sizeof(int));
    }

    // 执行并行波前计算
    for (round = 1; round <= m + n - 1; round++) {
        int thread_count = 0;

        for (i = 1; i <= m; i++) {
            j = round - i + 1;
            if (j >= 1 && j <= n) {
                thread_data[thread_count].i = i;
                thread_data[thread_count].j = j;
                thread_data[thread_count].X = X;
                thread_data[thread_count].Y = Y;
                thread_data[thread_count].L = L;
                thread_data[thread_count].mutex = &mutex;

                pthread_create(&threads[thread_count], NULL, lcs_thread, (void *)&thread_data[thread_count]);
                thread_count++;
            }
        }

        // 等待所有线程完成
        for (i = 0; i < thread_count; i++) {
            pthread_join(threads[i], NULL);
        }
    }

    int lcs_length = L[m][n];

    // 释放动态分配的内存
    for (i = 0; i <= m; i++) {
        free(L[i]);
    }
    free(L);

    pthread_mutex_destroy(&mutex);

    return lcs_length;
}

// 主函数
int main() {
    char X[100], Y[100];

    // 手动输入字符串X和Y
    printf("请输入字符串X: ");
    scanf("%s", X);
    printf("请输入字符串Y: ");
    scanf("%s", Y);

    int T;
    printf("请输入线程数量: ");
    scanf("%d", &T);
    if (T > MAX_THREADS) {
        printf("线程数量不能超过 %d.\n", MAX_THREADS);
        return 1;
    }

    int m = strlen(X);
    int n = strlen(Y);

    int lcs_length = lcs(X, Y, m, n, T);

    printf("LCS的长度是 %d\n", lcs_length);

    return 0;
}
