#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/timekeeping.h>
#include <linux/types.h>
#include <linux/file_monitor.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <linux/syscalls.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

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

/*
  Spinlock to protect:
  - monitored_path
  - log_buffer
  - log_count
*/
static DEFINE_SPINLOCK(file_monitor_lock);


void file_monitor_record_access(const char *path)
{
    unsigned long flags;
    bool is_buffer_full = false;
    bool access_recorded = false;
    if (path == NULL)
        return;

    spin_lock_irqsave(&file_monitor_lock, flags);

    // is there a file being monitored?
    if (monitored_path[0] == '\0') {
        spin_unlock_irqrestore(&file_monitor_lock, flags);
        return;
    }

    // if its not the monitored file
    if (strcmp(path, monitored_path) != 0) { // not equal
        spin_unlock_irqrestore(&file_monitor_lock, flags);
        return;
    }

    // Ignore new accesses when the buffer is full
    if (log_count >= FILE_MONITOR_MAX_LOGS) {
        is_buffer_full = true;
    } else {

        log_buffer[log_count].uid = current_uid();
        log_buffer[log_count].pid = current->pid;
        ktime_get_real_ts64(&log_buffer[log_count].ts);
        log_count++;
        access_recorded = true;

    }

    spin_unlock_irqrestore(&file_monitor_lock, flags);
    if (is_buffer_full)
        printk(KERN_INFO "file_monitor: buffer is full\n");
    if (access_recorded)
        printk(KERN_INFO "file_monitor: access recorded (pid = %d)\n", current->pid);
}

int file_monitor_set_path(const char *path)
{
    unsigned long flags;

    if (path == NULL)
        return -EINVAL;

    spin_lock_irqsave(&file_monitor_lock, flags);

    strncpy(monitored_path, path, FILE_MONITOR_MAX_PATH - 1);
    monitored_path[FILE_MONITOR_MAX_PATH - 1] = '\0';

    spin_unlock_irqrestore(&file_monitor_lock, flags);

    return 0;
}

int file_monitor_read_logs(char __user *buffer, size_t size)
{
    unsigned long flags;
    struct file_monitor_entry local_buffer[FILE_MONITOR_MAX_LOGS];
    int entries_to_copy, offset = 0, i;

    spin_lock_irqsave(&file_monitor_lock, flags);

    entries_to_copy = log_count;
    memcpy(local_buffer, log_buffer, log_count * sizeof(struct file_monitor_entry));
    log_count = 0;

    spin_unlock_irqrestore(&file_monitor_lock, flags);

    for(i = 0; i < entries_to_copy; ++i) {
        char line[128];
        int len;

        len = snprintf(
            line,
            sizeof(line),
            "pid = %d | uid = %u | ts = %lld.%09ld\n",
            local_buffer[i].pid,
            local_buffer[i].uid.val,
            (long long)local_buffer[i].ts.tv_sec,
            local_buffer[i].ts.tv_nsec);

        if (offset + len >= size)
            break;
        if (copy_to_user(buffer + offset, line, len))
            return -EFAULT;

        offset += len;
    }

    return offset;
}

SYSCALL_DEFINE1(set_monitored_file, const char __user *, path)
{
    char kpath[FILE_MONITOR_MAX_PATH];
    if (strncpy_from_user(kpath, path, FILE_MONITOR_MAX_PATH) < 0)
        return -EFAULT;
    return file_monitor_set_path(kpath);
}

SYSCALL_DEFINE2(read_file_monitor_logs, char __user *, buffer, size_t, size)
{
    return file_monitor_read_logs(buffer, size);
}

