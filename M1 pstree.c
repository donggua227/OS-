#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#define MAX_PROCS 1024

typedef struct Process {
    pid_t pid;
    pid_t ppid;
    char name[256];
} Process;

Process procs[MAX_PROCS];
int proc_count = 0;

void read_proc() {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir("/proc")) == NULL) {
        perror("opendir(/proc) failed");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char *endptr;
            pid_t pid = strtol(entry->d_name, &endptr, 10);
            if (*endptr == '\0') {  // if it's a number
                char path[256];
                sprintf(path, "/proc/%d/stat", pid);

                FILE *stat_file = fopen(path, "r");
                if (stat_file) {
                    Process p;
                    p.pid = pid;
                    fscanf(stat_file, "%d %s %*c %d", &p.pid, p.name, &p.ppid);
                    fclose(stat_file);
                    procs[proc_count++] = p;
                }
            }
        }
    }

    closedir(dir);
}

void print_tree(pid_t ppid, int indent, int is_last) {
    for (int i = 0; i < proc_count; i++) {
        if (procs[i].ppid == ppid) {
            for (int j = 0; j < indent; j++) {
                if (j == indent - 1) {
                    printf("%s", is_last ? "└──" : "├──");
                } else {
                    printf("│   ");
                }
            }
            printf("%s\n", procs[i].name);

            int has_child = 0;
            for (int k = 0; k < proc_count; k++) {
                if (procs[k].ppid == procs[i].pid) {
                    has_child = 1;
                    break;
                }
            }
            print_tree(procs[i].pid, indent + 1, !has_child);
        }
    }
}

int main(int argc, char *argv[]) {
    read_proc();
    print_tree(1, 0, 0);  // 从init进程（PID 1）开始
    return 0;
}
