#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/timekeeping.h>
#include <linux/types.h>
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
    /* TODO */
    return 0;
}

static int __init file_monitor_init(void)
{
    printk(KERN_INFO "file_monitor: initialized\n");
    return 0;
}

early_initcall(file_monitor_init);