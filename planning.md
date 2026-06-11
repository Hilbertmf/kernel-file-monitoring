# Goal

Build a small kernel subsystem that:

1. Monitors one file path.
2. Detects when that file is opened.
3. Records UID, PID and timestamp.
4. Stores records in a fixed-size kernel buffer.
5. Returns the logs through a syscall.
6. Clears the buffer after reading.

---

# Files to modify

## Existing files

### fs/open.c

Responsibility:

- Insert a hook in the file-open path.
- Keep changes minimal.
- Delegate logic to file_monitor.

Conceptually:

```text
file opened
    ↓
file_monitor_record_access(...)
```

---

### arch/x86/entry/syscalls/syscall_64.tbl

Responsibility:

- Register new syscalls.

Expected syscalls:

```text
set_monitored_file
read_file_monitor_logs
```

---

### include/linux/syscalls.h

Responsibility:

- Add syscall declarations.

---

### kernel/Makefile

Responsibility:

- Compile file_monitor.c.

Example:

```make
obj-y += file_monitor.o
```

---

## New files

### kernel/file_monitor.c

Main subsystem implementation.

Responsibilities:

- Store monitored path.
- Store audit/event buffer.
- Implement log insertion.
- Implement syscall logic.
- Expose helper functions.

Contains:

```text
monitored file path

fixed-size log buffer

record access logic

read logs logic

clear buffer logic
```

---

### include/linux/file_monitor.h

Shared declarations.

Responsibilities:

- Expose subsystem API.
- Avoid exposing implementation details.

Expected declarations:

```text
file_monitor_record_access()

file_monitor_set_path()

file_monitor_read_logs()
```

---

# Internal data structures

## Monitored file

Store a single path.

Example:

```text
/home/user/secret.txt
```

Only one monitored file is needed.

---

## Log entry

Each entry stores:

```text
UID
PID
timestamp
```

Exactly what the assignment requires.

---

## Buffer

Fixed-size array.

Example:

```text
64 entries
```

No dynamic allocation.

No linked lists.

Keep it simple.

---

# Syscalls

## set_monitored_file(path)

Purpose:

- Define which file path is being monitored.

Example:

```text
monitor /tmp/secret.txt
```

---

## read_file_monitor_logs(...)

Purpose:

- Return accumulated log entries.

After successful read:

```text
clear buffer
```

Required by the assignment.

---

# Monitoring flow

```text
open file
    ↓
monitoring enabled?
    ↓
yes
    ↓
path matches monitored path?
    ↓
yes
    ↓
collect:
    UID
    PID
    timestamp
    ↓
append entry to buffer
```

---

# Development order

## Phase 1

Subsystem skeleton.

Files:

```text
kernel/file_monitor.c
include/linux/file_monitor.h
kernel/Makefile
```

Goal:

```text
Compile successfully.
```

---

## Phase 2

Implement internal state.

Add:

```text
monitored path
log buffer
log entry structure
spinlock (protege acesso concorrente ao path e ao buffer)
```

Goal:

```text
Compile successfully.
```

---

## Phase 3

Implement logging helper.

Function:

```text
file_monitor_record_access()
```

Goal:

```text
Store UID, PID and timestamp in buffer.
```

---

## Phase 4

Hook into fs/open.c.

Goal:

```text
Detect file opens.

Call file_monitor_record_access().
```

Test using:

```text
printk
dmesg
```

---

## Phase 5

Implement configuration syscall.

Goal:

```text
Set monitored file path from userspace.
```

Files:

```text
syscall_64.tbl
syscalls.h
file_monitor.c
```

---

## Phase 6

Implement log-reading syscall.

Goal:

```text
Return accumulated logs.

Clear buffer after reading.
```

Files:

```text
syscall_64.tbl
syscalls.h
file_monitor.c
```

---

# Tools

## dmesg

Use to inspect kernel messages.

---

## printk

Use for debugging.

Example uses:

```text
hook reached

path matched

entry recorded
```

---

## strace

Use to verify userspace behavior and syscall execution.

---

# Demo video

1. Boot custom kernel.
2. Configure monitored file.
3. Open monitored file.
4. Read logs.
5. Show:
   - UID
   - PID
   - timestamp
6. Read logs again.
7. Show buffer is empty.