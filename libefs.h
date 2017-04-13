#include "efs.h"

/* FILE MODES for opening a file */
enum
{
  MODE_NORMAL=0, // Opens a file for read/write if it exists. New data will be written to the
                    // Front of the file.
  MODE_CREATE=1, // Create a new file and open it for read/write if it doesn't exist. New data will be
                    // written to the front of the file.
  MODE_READ_ONLY=2, // Opens a file in Read Only Mode if it exists
  MODE_READ_APPEND=3 // New data will be appended to the back of the file
};

/* Open File Table structure. Feel free to modify */
typedef struct oft
{
  unsigned char openMode; // Mode selected
  unsigned int blockSize; // Size of each block
  unsigned long inode; // Inode pointer
  unsigned long *inodeBuffer; // Inode buffer
  char *buffer; // Data buffer
  unsigned int writePtr; // Buffer index for writing data
  unsigned int readPtr; // Buffer index for reading data
  unsigned int filePtr; // File pointer. Points relative to ALL data in a file, not just the current buffer
} TOpenFile;

// Mounts a paritition given in fsPartitionName. Must be called before all
// other functions
void initFS(const char *fsPartitionName, const char *fsPassword);

// Opens a file in the partition. Depending on mode, a new file may be created
// if it doesn't exist, or we may get FS_FILE_NOT_FOUND in _result. See the enum above for valid modes.
// Return -1 if file open fails for some reason. E.g. file not found when mode is MODE_NORMAL, or
// disk is full when mode is MODE_CREATE, etc.

int openFile(const char *filename, unsigned char mode);

// Write data to the file. File must be opened in MODE_NORMAL or MODE_CREATE modes. Does nothing
// if file is opened in MODE_READ_ONLY mode.
// dataSize = size of each data unit. Use sizeof(.) to determine.
// dataCount = # of data units to write.
// Note dataSize * dataCount can exceed the size of one block.
void writeFile(int fp, void *buffer, unsigned int dataSize, unsigned int dataCount);

// Flush the file data to the disk. Writes all data buffers, updates directory,
// free list and inode for this file.
void flushFile(int fp);

// Read data from the file.
// dataSize = size of each data unit. Use sizeof(.) to determine.
// dataCount = # of data units to write.
// Note dataSize * dataCount can exceed the size of one block.
void readFile(int fp, void *buffer, unsigned int dataSize, unsigned int dataCount);

// Delete the file. Read-only flag (bit 2 of the attr field) in directory listing must not be set. 
// See TDirectory structure.
void delFile(const char *filename);

// Close a file. Flushes all data buffers, updates inode, directory, etc.
void closeFile(int fp);

// Unmount file system.
void closeFS();

