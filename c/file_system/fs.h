
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#define DEBUG_MODE 1
#define NON_DEBUG_MODE 0

#define MAX_FILE_ENTRIES 10000
#define MAX_FILE_NAME 100
#define MAX_FILE_SIZE 4096
#define FILE_SYSTEM_PATH "fs.bin"
#define TEST_FILE_SYSTEM_PATH "fs_test.bin"

// error codes
#define SAVE_FS_SUCCESS 0
#define SAVE_FS_OPEN_FILE_ERROR 1
#define SAVE_FS_WRITE_FILE_ERROR 2

#define LOAD_FS_SUCCESS 0
#define LOAD_FS_OPEN_FILE_ERROR 1
#define LOAD_FS_WRITE_FILE_ERROR 2
#define LOAD_FS_NULL_PTR_TO_FS 3

#define SUCCESS_CREATE_FILE 0
#define FAILURE_CREATE_FILE 1

#define REMOVE_FILE_SUCCESS 0
#define REMOVE_FILE_ERROR 1

#define WRITE_FILE_FAILURE 0

#define READ_FILE_FAILURE 0

#define true 1
#define false 0

// File entry holds info about file such as filename
typedef struct {
  char file_name[MAX_FILE_NAME];
  int size;
  int block_index;
} FILE_ENTRY;

typedef struct {
  char *blob[MAX_FILE_ENTRIES]; // so create a array of pointer, of size
                                // MAX_FILE_ENTRIES, and now for each file
                                // entries when we want to write we have to
                                // allocate memory
} STORAGE_BLOB;

// FILE_SYSTEM struct contains array of FILE_ENTRY
typedef struct {
  FILE_ENTRY files[MAX_FILE_ENTRIES];
  STORAGE_BLOB blob_storage;
} FILE_SYSTEM;

// save_fs takes pointer to FILE_SYSTEM and saves to disk as pointed by PATH
int save_fs(FILE_SYSTEM *fs, char *PATH) {
  FILE *file = fopen(PATH, "wb");
  if (file == NULL) {
    perror("could not open file");
    return SAVE_FS_OPEN_FILE_ERROR;
  }
  int r = fwrite(fs, sizeof(FILE_SYSTEM), 1, file);
  if (r == 0) {
    perror("unable to save file system");
    return SAVE_FS_WRITE_FILE_ERROR;
  }

  fclose(file);
  return SAVE_FS_SUCCESS;
}

// load_fs loads the file system into fs from the file pointed by PATH
int load_fs(FILE_SYSTEM *fs, char *PATH) {
  FILE *file = fopen(PATH, "rb");
  if (file == NULL) {
    perror("error while loading fs");
    return LOAD_FS_OPEN_FILE_ERROR;
  }
  if (fs == NULL) {
    perror("load_fs() File system file has been read but the file system type "
           "provided is NULL");
    return LOAD_FS_NULL_PTR_TO_FS;
  };

  fread(fs, sizeof(FILE_SYSTEM), 1,
        file); // fread looks at the memory layout where it is stroing and
               // correctly puts it in the fs pointer struct
  fclose(file);
  return LOAD_FS_SUCCESS;
}

// create_file creates a new entry  to the file *filename in fs.files
int create_file(FILE_SYSTEM *fs, char *filename) {
  for (int i = 0; i < MAX_FILE_ENTRIES; i++) {
    if (strcmp(fs->files[i].file_name, "\0") ==
        0) { // found where the filename has not been set, i.e. an empty slot
      strncpy(fs->files[i].file_name, filename,
              MAX_FILE_NAME); // direct assignment for string in C can not be
                              // done, so we are copying, also we do not want
                              // buffere overflow in user controlled value, so
                              // we use strncpy
      // todo
      // Update block index from storage block
      char *file_content = calloc(MAX_FILE_SIZE, sizeof(char));
      fs->blob_storage.blob[i] = file_content;
      fs->files[i].block_index = i;
      fs->files[i].size = MAX_FILE_SIZE;
      return SUCCESS_CREATE_FILE;
    }
  }
  return FAILURE_CREATE_FILE;
}

int remove_file(FILE_SYSTEM *fs, char *filename) {
  for (int i = 0; i < MAX_FILE_ENTRIES; i++) {
    if (strcmp(fs->files[i].file_name, filename) == 0) {
      // found
      // remove filename
      memset(fs->files[i].file_name, 0, MAX_FILE_NAME);
      // memset content to 0
      memset(fs->blob_storage.blob[i], 0, 4096);
      // free the allocated memory
      free(fs->blob_storage.blob[i]);

      fs->blob_storage.blob[i] = NULL;
      // todo update storage block and block index
      return REMOVE_FILE_SUCCESS;
    }
  }
  return REMOVE_FILE_ERROR;
}

int write_to_file(FILE_SYSTEM *fs, char *file_name, char *file_content) {
  for (int i = 0; i < MAX_FILE_ENTRIES; i++) {
    // found file
    if (strcmp(fs->files[i].file_name, file_name) == 0) {
      strncpy(fs->blob_storage.blob[i], file_content, MAX_FILE_SIZE);
      if (strlen(file_content) < MAX_FILE_SIZE) {
        return strlen(file_content);
      } else {
        return MAX_FILE_SIZE;
      }
    }
  }
  return WRITE_FILE_FAILURE;
}

char *read_from_file(FILE_SYSTEM *fs, char *file_name) {
  for (int i = 0; i < MAX_FILE_ENTRIES; i++) {
    // found file
    if (strcmp(fs->files[i].file_name, file_name) == 0) {
      return fs->blob_storage.blob[i];
    }
  }

  return NULL;
}

// file_exists checks if the file *filename exists in fs.files
int file_exists(FILE_SYSTEM *fs, char *filename) {
  for (int i = 0; i < MAX_FILE_ENTRIES; i++) {
    if (strcmp(fs->files[i].file_name, filename) == 0) {
      return true;
    }
  }
  return false;
}

// test_read_and_load_fs runs basic sanity test check
void test_read_and_load_fs(int DEBUG) {

  char file_name[] = "test.txt";
  char file_content[] = "This is a dummy file content";
  // Act - 1: Create a file system
  FILE_SYSTEM *fs = calloc(1, sizeof(FILE_SYSTEM));
  if (fs == NULL) {
    perror(
        "test_read_and_load_fs(): unable to calloc FILE_SYSTEM for testing\n");
    exit(-1);
  }

  if (DEBUG) {
    printf("FILE system created successfully \n");
  }

  // Act - 2: Create a file in that file system
  if (create_file(fs, file_name) != SUCCESS_CREATE_FILE) {
    perror("test_read_and_load_fs(): unable to create_file\n");
    exit(-1);
  }

  if (DEBUG) {
    printf("file created in FILE_SYSTEM\n");
  }

  // Act -3 : Save the file system;
  if (save_fs(fs, TEST_FILE_SYSTEM_PATH) != SAVE_FS_SUCCESS) {
    perror("test_read_and_load_fs(): unable to save file system\n");
    exit(-1);
  }

  if (DEBUG) {
    printf("FILE_SYSTEM saved successfully\n");
  }
  // ACt -4: Celean the file system
  free(fs);

  // Act -5: load the file system
  fs = calloc(1, sizeof(FILE_SYSTEM));
  if (fs == NULL) {
    perror("test_read_and_load_fs() unable to callof FILE_SYSTEM for ACT 5\n");
    exit(-1);
  }

  if (load_fs(fs, TEST_FILE_SYSTEM_PATH) != LOAD_FS_SUCCESS) {
    perror("test_read_and_load_fs(): unable to load_fs\n");
    exit(-1);
  }
  if (DEBUG) {
    printf("FILE systesm loaded successfully\n");
  }

  // ACt -6 Check if the file is present in the file system
  if (!file_exists(fs, file_name)) {
    perror(
        "test_read_and_load_fs(): test.txt file should  exist but does not\n");
    exit(-1);
  }

  // Act - 7: Write to file
  if (DEBUG) {
    printf("test_read_and_load_fs(): attemtpting to write to a file\n");
  }
  if (write_to_file(fs, file_name, file_content) == WRITE_FILE_FAILURE) {
    perror("test_read_and_load_fs(): writing to file failure");
    exit(-7);
  }
  if (DEBUG) {
    printf("Written to file successfully\n");
  }
  // Act - 8: Check if file content matched
  char *content = read_from_file(fs, file_name);
  if (content == NULL) {
    perror("test_read_and_load_fs(): read_from file returned NULL pointer");
    exit(-8);
  }

  if (strcmp(content, file_content) != 0) {
    perror("test_read_and_load_fs(): file contents are not same");
    exit(-1);
  }
  if (DEBUG) {
    printf("File content %s \n", content);
  }

  // ACt - 9: Remove the file
  if (remove_file(fs, file_name) != REMOVE_FILE_SUCCESS) {
    perror("test_read_and_load_fs(): could not remove file\n");
    exit(-7);
  }
  if (DEBUG) {
    printf("file %s was removed successfully\n", file_name);
  }

  // ACt - 10: Check if removed file exist
  if (file_exists(fs, file_name)) {
    perror("test_read_and_load_fs(): file should not exist after removal but "
           "does\n");
    exit(-8);
  }
  if (DEBUG) {
    printf("test_read_and_load_fs(): file remove test case passed\n");
  }
  printf("test_read_and_load_fs(): SUCCESS\n");

  // check if file content still exists
  if (read_from_file(fs, file_name) != NULL) {
    perror("test_read_and_load_fs(): file content should now be null\n");
  }
  if (DEBUG) {
    printf("test_read_and_load_fs(): file content was cleaned successfully\n");
  }
  // ACt - 11: Cleanup
  if (remove(TEST_FILE_SYSTEM_PATH) != 0) {
    perror("test_read_and_load_fs(): error during cleanup");
  }
  free(fs);
}
