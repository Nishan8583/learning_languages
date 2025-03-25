
✅ File Descriptor (FD) Flow in Linux:
File Descriptor Table (per process):

When your C program opens a file (e.g., using open() or fopen()), the kernel creates an entry in the file descriptor table for your process.

The FD table maps FDs (integers) to file table entries.

File Table (system-wide):

The file table entry contains information like:

A pointer to the inode (for the actual file).

The current file offset (for lseek() operations).

Open flags (e.g., O_RDONLY, O_WRONLY).

Reference count (for shared FDs).

Multiple FDs (from different processes) can point to the same file table entry if they share an open file.

Inode Table (system-wide):

The inode (index node) represents metadata about the file, such as:

File type, permissions, and ownership.

File size.

Timestamps (last accessed, modified).

Pointers to the data blocks on disk.

The inode does not store the filename—it only references the file content.


[Process FD Table]
+-----+                     +-------------+                       +--------------+
|  3  | --> Regular File --> | File Table  | --> Inode Table      | Data Blocks  |
+-----+                     | Offset=0    |                      | Disk Storage |
                            | Flags=RDWR  |                      +--------------+
                            +-------------+
                            | Inode=1234  |
                            +-------------+

+-----+                     +--------------+                      +-----------------------+
|  4  | --> Socket FD -----> | Socket Table | --> Protocol Control | Local/Remote IP:Port   |
+-----+                     | Proto=TCP    |    Block (PCB)        | TCP state, buffer info |
                            | State=ESTAB  |                      | Window size, etc.      |
                            +--------------+                      +-----------------------+

+-----+                     +-------------+                       +------------------+
|  5  | --> Device FD -----> | File Table  | --> Inode Table       | Device Driver     |
+-----+                     | Offset=0    |                       | (e.g., SATA disk) |
                            | Flags=RDWR  |                       | Major=8, Minor=0  |
                            +-------------+                       +------------------+
                            | Inode=5678  |
                            +-------------+
