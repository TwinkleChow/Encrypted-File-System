#include "efs.h"

char *bitmap;
TDirectory *directory;
unsigned long **inodeTable;

int main(int ac, char **av)
{
  if(ac<2)
  {
    fprintf(stderr, "\nUsage: %s <config filename>\n\n", av[0]);
    return -1;
  }

  TFileSystemStruct fs;
  int i;

  FILE *fp;

  fp = fopen(av[1], "r");
  if(fp == NULL)
  {
    fprintf(stderr, "Unable to open config file.\n");
    return -1;
  }

  /* Read the config file */
  char partName[32];
  fscanf(fp, "%s\n", partName);
  fscanf(fp, "%lu\n", &fs.fsSize);
  fscanf(fp, "%d\n", &fs.blockSize);
  fscanf(fp, "%d\n", &fs.maxFiles);
  fclose(fp);

  directory = (TDirectory *) calloc(sizeof(TDirectory), fs.maxFiles);

  for(i = 0; i<fs.maxFiles; i++)
  {
    strcpy(directory[i].filename, "nofile.dat");
    directory[i].attr=0;
  }

  fs.fsSize  = fs.fsSize * 1024 * 1024;

  // Start calculating all the byte indexes
  fs.numBlocks = fs.fsSize / fs.blockSize;

  // Allocate the bitmap
  fs.bitmapLen = ceil(fs.numBlocks / double(8));
  bitmap = (char *) calloc(sizeof(char), fs.bitmapLen);
  memset(bitmap, 0xff, fs.bitmapLen);

  // # of entries per inode block
  fs.numInodeEntries = fs.blockSize / sizeof(unsigned long);

  // Allocate the inode table
  inodeTable = (unsigned long **) calloc(sizeof(unsigned long), fs.maxFiles);

  for(i=0; i<fs.maxFiles; i++)
    inodeTable[i] = (unsigned long *) calloc(sizeof(unsigned long), fs.numInodeEntries);

  // Directory begins after the metadata
  fs.dirByteIndex = sizeof(TFileSystemStruct);

  // Bitmap begins after the directory, which is size of each entry * maxfiles
  fs.bitmapByteIndex = fs.dirByteIndex + sizeof(TDirectory) * fs.maxFiles;

  // inode table begins after bitmaps
  fs.inodeByteIndex = fs.bitmapByteIndex + fs.bitmapLen;

  // Data table begins after inode table. There is one inode per file, and each inode is one block
  fs.dataByteIndex = fs.inodeByteIndex + fs.blockSize * fs.maxFiles;

  // Usable data space
  unsigned long usableSpace = fs.fsSize - fs.dataByteIndex + 1;

  FILE *outfp;

  outfp = fopen(partName, "w");

  // Write out the file FS descriptor
  fwrite(&fs, sizeof(fs), 1, outfp);

  // Write out the directory
  fseek(outfp, fs.dirByteIndex, SEEK_SET);
  fwrite(directory, sizeof(TDirectory), fs.maxFiles, outfp);

  // Seek to start of bitmap table
  fseek(outfp, fs.bitmapByteIndex, SEEK_SET);

  // Write the bitmap
  fwrite(bitmap, fs.bitmapLen, 1, outfp);

  // Write the inode table
  fseek(outfp, fs.inodeByteIndex, SEEK_SET);
  for(i=0; i<fs.maxFiles; i++)
    fwrite(inodeTable[i], sizeof(unsigned long), fs.numInodeEntries, outfp);

  // Write out the data
  fseek(outfp, fs.fsSize, SEEK_SET);
  fprintf(outfp, "!");
  fclose(outfp);

  printf("\nFILE SYSTEM SUMMARY\n");
  printf(  "===================\n\n");
  printf("File system size: %lu bytes\n", fs.fsSize);
  printf("Block size: %u bytes\n", fs.blockSize);
  printf("Maximum number of files: %u\n", fs.maxFiles);
  printf("Number of blocks: %u\n", fs.numBlocks);
  printf("Bitmap Length: %u bytes\n", fs.bitmapLen);
  printf("Usable Data Space: %lu bytes\n", usableSpace);
  printf("Number of pointers per inode block: %u\n", fs.numInodeEntries);
  printf("Percentage Usable Data Space: %3.2g%%\n", (double) usableSpace / fs.fsSize * 100.0);

  printf("\nByte Indexes:\n\n");

  printf("Directory Index: %u\n", fs.dirByteIndex);
  printf("Bitmap Index: %u\n", fs.bitmapByteIndex);
  printf("Inode Index: %u\n", fs.inodeByteIndex);
  printf("Data Index: %u\n\n", fs.dataByteIndex);

  free(directory);
  free(bitmap);

  for(i=0; i<fs.maxFiles; i++)
    free(inodeTable[i]);

  free(inodeTable);
}

