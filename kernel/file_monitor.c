#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/timekeeping.h>

#include <linux/file_monitor.h>

#define FILE_MONITOR_MAX_PATH 256
#define FILE_MONITOR_MAX_LOGS 64

struct file_monitor_entry {
    kuid_t uid;
    pid_t pid;
    struct timespec64 ts;
};

static char monitored_path[FILE_MONITOR_MAX_PATH];

static struct file_monitor_entry log_buffer[FILE_MONITOR_MAX_LOGS];
static int log_count;

void file_monitor_record_access(const char *path)
{
    /* TODO */
}

int file_monitor_set_path(const char *path)
{
    /* TODO */
    return 0;
}

int file_monitor_read_logs(char __user *buffer, size_t size)
{
    /* TODO */
    return 0;
}