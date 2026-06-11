
#ifndef _LINUX_FILE_MONITOR_H
#define _LINUX_FILE_MONITOR_H

#include <linux/types.h>
#include <linux/uaccess.h>

void file_monitor_record_access(const char *path);

int file_monitor_set_path(const char *path);

int file_monitor_read_logs(char __user *buffer, size_t size);

#endif