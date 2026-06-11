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

/*
  Spinlock to protect:
  - monitored_path
  - log_buffer
  - log_count
 */
static DEFINE_SPINLOCK(file_monitor_lock);


void file_monitor_record_access(const char *path)
{
    /* TODO */
}

int file_monitor_set_path(const char *path)
{
	unsigned long flags;

	if (!path)
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