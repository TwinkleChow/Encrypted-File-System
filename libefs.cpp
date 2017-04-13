#include "libefs.h"

#define max_no 100

// FS Descriptor
TFileSystemStruct *_fs;

// Open File Table
TOpenFile* _oft[max_no];

int oft_available[max_no] = { 0 };

// Open file table counter
int _oftCount = 0;

// Mounts a paritition given in fsPartitionName. Must be called before all
// other functions
void initFS(const char *fsPartitionName, const char *fsPassword) {

	mountFS(fsPartitionName, fsPassword);
}

// Opens a file in the partition. Depending on mode, a new file may be created
// if it doesn't exist, or we may get FS_FILE_NOT_FOUND in _result. See the enum above for valid modes.
// Return -1 if file open fails for some reason. E.g. file not found when mode is MODE_NORMAL, or
// disk is full when mode is MODE_CREATE, etc.

int openFile(const char *filename, unsigned char mode) {
	if ((mode == MODE_NORMAL) or (mode == MODE_CREATE)
			or (mode == MODE_READ_ONLY) or (mode == MODE_READ_APPEND)) {
		unsigned long result = findFile(filename);


		if (result != FS_FILE_NOT_FOUND) {

			//the file exists
			//result stores the directory index
			//create entry in the open file table
			int index_to_add = -1;
			for (int i = 0; i < max_no; i++) {
				if (oft_available[i] == 0) {
					index_to_add = i;
				}

			}
			if (index_to_add == -1) {
				printf("error!The oft is full");
				return -1;
			} else {

				_oft[index_to_add] = new TOpenFile;
				_oft[index_to_add]->openMode = mode;
				_oft[index_to_add]->blockSize = _fs->blockSize;
				_oft[index_to_add]->inode = result;
				_oft[index_to_add]->inodeBuffer = makeInodeBuffer();
				//load inode
				loadInode(_oft[index_to_add]->inodeBuffer, result);

				_oft[index_to_add]->buffer = makeDataBuffer();

				if (mode == MODE_READ_APPEND) {
					unsigned long len = getFileLength(filename);

					_oft[index_to_add]->filePtr = len;

				} else {
					_oft[index_to_add]->writePtr = 0;
					_oft[index_to_add]->readPtr = 0;
					_oft[index_to_add]->filePtr = 0;
				}
				//make this entry as used
				oft_available[index_to_add] = 1;
				_oftCount++;

				return (index_to_add);

			}
		}

		else {

			//file does not exist

			if (mode != MODE_CREATE) {
				return -1;
			} else {
				//mode == MODE_CREATE

				//create the file

				unsigned int index = makeDirectoryEntry(filename, 0x0, 0);	//?



				if (index != FS_DIR_FULL) {

					//allocate memory
					// Find a free block
					unsigned long freeBlock = findFreeBlock();

					// Mark the free block now as busy
					markBlockBusy(freeBlock);

					// Create the inode buffer
					unsigned long *inode = makeInodeBuffer();

					// Load the
					loadInode(inode, index);

					// Set the first entry of the inode to the free block
					inode[0]=freeBlock;

					// Write the inode
					saveInode(inode, index);

					// Write the free list
					updateFreeList();

					// Write the diretory
					updateDirectory();

					//fiel created, create an open file entry

					int index_to_add = -1;
					for (int i = 0; i < max_no; i++) {
						if (oft_available[i] == 0) {
							index_to_add = i;
						}

					}
					if (index_to_add == -1) {
						printf("error!The oft is full");
						return -1;
					}

					else {
						_oft[index_to_add] = new TOpenFile;
						_oft[index_to_add]->openMode = mode;
						_oft[index_to_add]->blockSize = _fs->blockSize;
						_oft[index_to_add]->inode = index;
						_oft[index_to_add]->inodeBuffer = makeInodeBuffer();
						//load inode
						loadInode(_oft[index_to_add]->inodeBuffer, index);
						_oft[index_to_add]->buffer = makeDataBuffer();

						_oft[index_to_add]->writePtr = 0;
						_oft[index_to_add]->readPtr = 0;
						_oft[index_to_add]->filePtr = 0;

						_oftCount++;

						return (index_to_add);
					}
				}

				else {
					//disk is full
					return -1;
				}

			}
		}

	}

	else {
		//mode invalid
		return -1;

	}

}

// Write data to the file. File must be opened in MODE_NORMAL or MODE_CREATE modes. Does nothing
// if file is opened in MODE_READ_ONLY mode.
void writeFile(int fp, void *buffer, unsigned int dataSize,
		unsigned int dataCount) {

	unsigned char mode = _oft[fp]->openMode;

	if (mode != MODE_READ_ONLY) {

		//write data to the file
		for (int i = 0; i < dataSize * dataCount; i++) {
			if (_oft[fp]->writePtr < _oft[fp]->blockSize) {
				//write to the buffer
//				_oft[fp]->buffer[_oft[fp]->writePtr] = buffer[i];
				memcpy(_oft[fp]->buffer + _oft[fp]->writePtr, buffer + i, 1);
				_oft[fp]->writePtr++;
				_oft[fp]->filePtr++;

			} else {
				//writePtr == blockSize
				unsigned long blockNum = returnBlockNumFromInode(
						_oft[fp]->inodeBuffer, _oft[fp]->filePtr);

				if (!blockNum) {
					//block has not yet been allocated for this byte offset
					//find a free block
					unsigned long newblockNum = findFreeBlock();

					if (newblockNum != FS_FULL) {
						setBlockNumInInode(_oft[fp]->inodeBuffer,
								_oft[fp]->filePtr, newblockNum);
						blockNum = newblockNum;
						markBlockBusy(blockNum);
						updateFreeList();


					} else {
						printf("fail because of FS_FULL");
						exit(-1);
					}
				}
				//write data to the disk

				writeBlock(_oft[fp]->buffer, blockNum);
				saveInode(_oft[fp]->inodeBuffer,_oft[fp]->inode);
				_oft[fp]->writePtr = 0;
				updateDirectory();

			}

		}

	} else {
		//mode ==MODE_READ_ONLY
		printf("MODE_READ_ONLY,does nothing");
	}

}

// Flush the file data to the disk. Writes all data buffers, updates directory,
// free list and inode for this file.
void flushFile(int fp) {

	if (_oft[fp]->writePtr) {
		//flush the file data to the disk
		unsigned long blockNum = returnBlockNumFromInode(_oft[fp]->inodeBuffer,
				_oft[fp]->filePtr);

		if (!blockNum) {
			//block has not yet been allocated for this byte offset
			//find a free block
			unsigned long newblockNum = findFreeBlock();

			if (newblockNum != FS_FULL) {
				setBlockNumInInode(_oft[fp]->inodeBuffer, _oft[fp]->filePtr,
						newblockNum);
				blockNum = newblockNum;
				markBlockBusy(blockNum);
				updateFreeList();

			} else {
				printf("fail because of FS_FULL");
				exit(-1);
			}
		}
		//write data to the disk

		writeBlock(_oft[fp]->buffer, blockNum);
		_oft[fp]->writePtr = 0;

		//update directory and length of the file(?)
		updateDirectory();
//		updateDirectoryFileLength(_oft[fp]->fileName,_oft[fp]->filePtr);
		saveInode(_oft[fp]->inodeBuffer, _oft[fp]->inode);
	}

}

// Read data from the file.
void readFile(int fp, void *buffer, unsigned int dataSize,
		unsigned int dataCount) {
	buffer = (char*) malloc(dataSize * dataCount);

	for (int i = 0; i < dataSize * dataCount; i++) {
		if (_oft[fp]->readPtr < _oft[fp]->blockSize) {
			//read to the buffer
//			 buffer[i]=_oft[fp]->buffer[_oft[fp]->filePtr];
			memcpy(buffer + i, _oft[fp]->buffer + _oft[fp]->filePtr, 1);
			_oft[fp]->readPtr++;
			_oft[fp]->filePtr++;

		} else {
			//readPtr == blockSize
			//read new block to the databuffer
			unsigned long blockNum = returnBlockNumFromInode(
					_oft[fp]->inodeBuffer, _oft[fp]->filePtr);

			if (blockNum) {
				readBlock(_oft[fp]->buffer, blockNum);

			} else {

				printf("End of the file");
			}

			_oft[fp]->readPtr = 0;

		}

	}

}

// Delete the file. Read-only flag (bit 2 of the attr field) in directory listing must not be set. 
// See TDirectory structure.
void delFile(const char *filename) {
	unsigned int ndx = findFile(filename);
	if (ndx != FS_FILE_NOT_FOUND) {
		unsigned int bit = (getattr(filename) >> 2) & 0b1;

		if (!bit) {
			delDirectoryEntry(filename);
		}
	}

}

// Close a file. Flushes all data buffers, updates inode, directory, etc.
void closeFile(int fp) {

	flushFile(fp);

	//free the buffer
	free(_oft[fp]->buffer);
	free(_oft[fp]->inodeBuffer);

	//delete the entry in the open file table
	oft_available[fp]=0;

}

// Unmount file system.
void closeFS() {
	unmountFS();

}
;

