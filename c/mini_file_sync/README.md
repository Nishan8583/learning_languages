# Some notes I ended up when trying to understand why we do things a certain way

# 🧠 Why unlink() is used to delete a file
✅ unlink() removes a name from the filesystem:

From the man page of unlink(2):

    The unlink() function deletes a name from the filesystem. If that name was the last link to a file and no processes have the file open, the file is deleted.

So:

    If it's a regular file or symbolic link, unlink() is the standard way to delete it.

    If it's a directory, unlink() won't work (you must use rmdir() instead).

🚫 Why not use remove() instead?

You could — remove() is a standard C library function that:

    Calls unlink() on files

    Calls rmdir() on directories

But:

    unlink() gives you more control and finer error handling

    It aligns with Unix system-level APIs (like open, stat, read, etc.)

🧾 Summary: When to Use What
File Type	Function to Delete	Notes
Regular file	unlink()	Deletes the file (if no open refs)
Symbolic link	unlink()	Deletes the link, not the target
Directory	rmdir()	Only works on empty directories
Any (generic)	remove()	Works for both, less control

So in your recursive directory deletion code:

    unlink() is the correct and POSIX-compliant way to delete a file

    rmdir() is used to remove an empty directory

Let me know if you'd like to handle special cases like sockets, FIFOs, or broken symlinks.


### Plan to add tests
- test.c
- it createts src and dst directory
- create some sample files in src directory
- Test case 1: Sync when no files were copied
- Test case 2: Update the some files in sync, and check if the files were copied
- Test case 3: When there is no changes, do not update the file
- A function that does the entire sync process, it takes src and dst folder name
