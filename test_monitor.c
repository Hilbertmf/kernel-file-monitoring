#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define SYS_set_monitored_file 441
#define SYS_read_file_monitor_logs 442

void set_path(char *path) {
    if (path == NULL) {
        fprintf(stderr, "set_path: path is NULL\n");
        return;
    }

    printf("monitoring file at %s\n", path);
    long result = syscall(SYS_set_monitored_file, path);
    if (result < 0)
        fprintf(stderr, "set_monitored_file failed: %ld\n\n", result);
    else
        printf("set_monitored_file returned: %ld\n\n", result);
    return;
}

void read_logs(char *buffer, size_t size) {
    if (buffer == NULL) {
        fprintf(stderr, "read_logs: buffer is NULL\n");
        return;
    }
    memset(buffer, 0, size);
    printf("reading logs...\n");
    long num_bytes = syscall(SYS_read_file_monitor_logs, buffer, size);
    printf("read_file_monitor_logs returned: %ld bytes\n\n", num_bytes);
    
    if (num_bytes > 0) {
        printf("log:\n%s\n", buffer);
        printf("--------------------------------------------------\n");
    } else if (num_bytes == 0) {
        printf("buffer is empty\n");
        printf("--------------------------------------------------\n");
    } else {
        fprintf(stderr, "read_file_monitor_logs failed: %ld\n\n", num_bytes);
        return;
    }
}

void open_file(char *path) {
    if (path == NULL) {
        fprintf(stderr, "open_file: path is NULL\n");
        return;
    }

    printf("opening file...\n");
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "open_file: failed to open %s\n", path);
        return;
    }
    close(fd);
}

int main() {
    char path[256], buffer[4096];
    printf("enter a file path to monitor with absolute path: ");
    scanf("%s", &path);
    set_path(path);
    
    read_logs(buffer, sizeof(buffer));    

    for(int i = 0; i < 10; ++i) {
        open_file(path);
    }
    printf("\n");
    
    read_logs(buffer, sizeof(buffer));
    printf("reading again to confirm buffer was cleared:\n");
    read_logs(buffer, sizeof(buffer));

    return 0;
}