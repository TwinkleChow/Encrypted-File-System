#include "libefs.h"

// FS Descriptor
TFileSystemStruct *_fs;

// Open File Table
TOpenFile *_oft;

// Open file table counter
int _oftCount=0;

// Mounts a paritition given in fsPartitionName. Must be called before all
// other functions
void initFS(const char *fsPartitionName, const char *fsPassword)
{

	mountFS(fsPartitionName,fsPassword);
}

// Opens a file in the partition. Depending on mode, a new file may be created
// if it doesn't exist, or we may get FS_FILE_NOT_FOUND in _result. See the enum above for valid modes.
// Return -1 if file open fails for some reason. E.g. file not found when mode is MODE_NORMAL, or
// disk is full when mode is MODE_CREATE, etc.

int openFile(const char *filename, unsigned char mode)
{
	if(mode==MODE_NORMAL){
			unsigned long result = findFile(filename);

			if (result != FS_FILE_NOT_FOUND){

				//result stores the directory index
				//create entry in the open file table
				_oft[_oftCount]= new TOpenFile;
				_oft[_oftCount].openMode = mode;
				_oft[_oftCount].blockSize = _fs.blockSize;
				_oft[_oftCount].inode = result;
				_oft[_oftCount].inodeBuffer = makeInodeBuffer();
				//load inode
				loadInode(_oft[_oftCount].inodeBuffer,result);

				_oft[_oftCount].buffer = makeDataBuffer();
				unsigned long byteNum;
				returnBlockNumFromInode(_oft[_oftCount].inodeBuffer,byteNum);
				readBlock(_oft[_oftCount].buffer,)

				readBlock(_oft[_oftCount].inodeBuffer,)


			}


	}

	return -1;
}

// Write data to the file. File must be opened in MODE_NORMAL or MODE_CREATE modes. Does nothing
// if file is opened in MODE_READ_ONLY mode.
void writeFile(int fp, void *buffer, unsigned int dataSize, unsigned int dataCount)
{


}

// Flush the file data to the disk. Writes all data buffers, updates directory,
// free list and inode for this file.
void flushFile(int fp)
{
}

// Read data from the file.
void readFile(int fp, void *buffer, unsigned int dataSize, unsigned int dataCount)
{
}

// Delete the file. Read-only flag (bit 2 of the attr field) in directory listing must not be set. 
// See TDirectory structure.
void delFile(const char *filename);

// Close a file. Flushes all data buffers, updates inode, directory, etc.
void closeFile(int fp);

// Unmount file system.
void closeFS();

