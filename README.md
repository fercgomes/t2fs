# T2FS

INF UFRGS - Operating Systems 1 Coursework

## Important notes

 * Block address begins in zero, and the first position is at the 
    ![equation](https://latex.codecogs.com/gif.latex?\Delta%20:=%20uperBlockSize%20+%20freeBlocksBitmapSize%20+%20freeInodeBitmapSize) Block
 * Any sector that coudln't fit in a Block will be ignored (**Only the ones at the end of the partition**).
 * THE Directory is described by one, and only one, i-node.

## The System Structure is composed of:
   * SUPERBLOCK - Super Block structure
   * THEDIR		- THE Directory structure
   * SWOFL		- System Wide Open File List
   * PWOFL		- Process Wide Open File List
 
## Implementation plan

#### Always create a test for a function or parts of a function!
#### Before starting to implement any function, _be sure the description is correct and complete_, and that you understand what it is supposed to be implemented.
#### Files must be saved in some cases. Keep it in mind, as it may not have been described.

### TODO
### 0. Initialization
   1. "SUPEBLOCK := NULL"
   2. "THEDIR    := NULL"
   3. "SWOFL     := NULL"
   4. "PWOFL     := NULL"
   
### 1. ~Format2~  
   1. Do the necessary calculations and create the Superblock
      * Calculations:
         * Sector Size: 256 Bytes. **It uses one whole Block in the system**
         * Sectors per Block: x Sectors, ![equation](https://latex.codecogs.com/gif.latex?x\in\(0,?],%20x\in\mathbb{Z})
         * Block size: ![equation](https://latex.codecogs.com/gif.latex?B_s%20=%20256*x) Bytes
         * Partition Size: ![equation](https://latex.codecogs.com/gif.latex?P_s%20=%20\(partitionXXLastBlockAddr%20-%20partitionXXFirstBlockAddr\))  Bytes
         * Blocks per Partition: ![equation](https://latex.codecogs.com/gif.latex?B_P%20=%20P_s/B_s) Blocks
         * Blocks for i-nodes: ![equation](https://latex.codecogs.com/gif.latex?B_I%20=%20\lceil{0.1*B_P}\rceil) Blocks
         * Blocks for i-node bitmap: ![equation](https://latex.codecogs.com/gif.latex?Bit_I%20=%20(M_I/8)/B_s) Blocks
            * i-nodes per Block: ![equation](https://latex.codecogs.com/gif.latex?I_B%20=%20B_s/32) i-nodes
            * Maximum i-nodes per Partition: ![equation](https://latex.codecogs.com/gif.latex?M_I%20=%20B_I*I_B) i-nodes
         * Blocks for Free/Used Blocks bitmap: ![equation](https://latex.codecogs.com/gif.latex?Bit_B%20=%20\(B_P/8\)/B_s) Blocks  
            **Attention: it's necessary to consider the meta-data blocks too, since the Superblock structure doesn't provide any information about the first sector of the data area.**
      * Checksum function
   2. Allocate the Free/Used Blocks bitmap and initialize it: 1 indicates occupied, 0 indicates free  
      **Attention: since the Superblock structure doesn't provide any information about the first sector of the data area, the whole meta-data area codified in this bitmap must be marked as used.**
   3. Allocate the i-node bitmap and initialize it: 1 indicates occupied, 0 indicates free.
   4. The Data Blocks will extend until the end of the partition.  
      **Important: these Blocks will be used for the data files, the diretory files and the index blocks.**   
   
### x. Common functions (incomplete):
   * Localize a Free Block.
      * Returns the Allocated Block ID.
   * Allocate a Free Block.
      * Returns the Allocated Block ID.
   * Deallocate an Occupied Block.
      * Returns Success or Failure.
   * Localize a Free i-node.
      * Returns the Free i-node ID.
   * Allocate a Free i-node.
      * Returns the Free i-node ID.
   * Deallocate an Occupied i-node.
      * Returns Success or Failure.
   * Function to create an i-node given the necessary informations.
      * Returns the i-node ID.
   * Function to deallocate all Blocks of a given i-node.
      * Returns Success or Failure (determined through the file size in bytes and number of Blocks deallocated).
   * Function to read a Block and return it in a buffer.
      * Returns Success or Failure.
   * Function to read a given number of Blocks and return them in a buffer.
      * Returns number of bytes read.
   * Function to write a given Block buffer into a Block position.
      * Returns number of bytes written.
   * Function to write a given number of Blocks provided in a buffer and given a position and a Size "s".
      * Returns number of bytes written.
   * ~Load Superblock~  
      * Returns zero if successful, any non-zero value otherwise.
   * ~Check if is a Superblock~  
      * Returns zero if is a Superblock, any non-zero value otherwise.
   * ~Checksum~  
      * Returns the value of the checksum.
   * ~Check if partition is mounted~ 
      * Returns non-zero if mounted, zero if not mounted.
   * ~Check if THE Director is open~  
      * Returns non-zero if is open, zero otherwise.
   * ~Initialization of the System Wide Open File List~  
      * Returns zero if success, non-zero if failure.
   * ~Destroy System Wide Open File List~  
      * Returns zero if success, non-zero if failure.
   * ~Initialization of the Process Wide Open File List~  
      * Returns zero if success, non-zero if failure.
   * ~Destroy Process Wide Open File List~  
      * Returns zero if success, non-zero if failure.
   * ~Create and Delete functions for both (separate), SWOFL entries and PWOFL entries~  
      * Returns zero if success, non-zero if failure.
   * ~Generate File Id~  
      * Returns the next available ID to describe the PWOFL file entry.
   * ~Check if a given filename is valid~  
      * Return zero is it is valid, non-zero otherwise.
   * ~Discover and retrieves a open file by its filename~  
      * Returns zero if found an open file with the given filename, non-zero otherwise.

### 2. How to implement THE Directory? i-node
 
### 3. ~Mount:~  
   1. See if there isn't any mounted partition. Only mount if the root is free to go. (Id 6: this repeats in another function)
   2. Read Superblock to RAM.
   3. Verify Checksum.
   4. Return Success.

### 4. ~Umount:~  
   0. Check if a partition is mounted. (Id 6: this repeats in another function)
   1. Check that no files are open, including THE Directory.
   2. ~Close THE Directory.~ Do not let unmount if directory is open.
   3. Deallocate Superblock.
   4. Return Success.
   
### 5. Identify: just do it.

### 6. Create2:  
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the given file name is valid. (Id 9: this repeats in another function)
   2. Check if a file with the given name is not open. If is open, return an error.
   3. Check if a file with the given name exists.
      1. In case it exists:
         1. Deallocate all its Blocks in the Free/Occupied Blocks Bitmap. (Id 8: this repeats in another function)  
            **Attention: remember to deallocate every block in all the given indirection levels.**  
         2. Update the i-node file size.
      2. In case it doesn't exists:
         1. Allocate an i-node and a Free Block. **Use predefined functions! Or create them!**
         2. Fill the i-node with the necessary info. **Use predefined functions! Or create them!**
         3. Create a dentry and add it to THE Directory. **Create a function for common file dentry creation!**
   4. Return Success
   
### 7. Delete2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the given file name is valid. (Id 9: this repeats in another function)
   2. Locate the file. (Id 1: this repeats in another function)
   3. Check that the file is not open.
   4. Load i-node.
   5. Decrement the "RefCounter" of the i-node.
   6. If the "RefCounter" reaches zero:
      1. Case true:
         1. Deallocate the Occupied i-node in the i-node Bitmap.
         2. Deallocate all its Blocks in the Free/Occupied Blocks Bitmap. (Id 8: this repeats in another function)  
            **Attention: remember to deallocate every block in all the given indirection levels.**  
         **Consider creating a function for this.**
         3. Unload i-node.
      2. Case false:
         1. Save i-node.
   7. Return Success. 
      
### 8. Open2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the given file name is valid. (Id 9: this repeats in another function)
   2. Locate the file. (Id 1: this repeats in another function)  
      **Attention: softlinks must be treated accordingly.**
      1. Search in the System-Wide Open File List.
      2. Search in THE Directory.
   3. Create FILE2 descriptor  
      **Needs a function: Create File Handler --- This makes it easier to test.**
      1. If doesn't exist, create an entry in the System-Wide Open File List (SWOFL) with the file name, i-node address and number of references **(this entry is a structure)**.
      2. Create an entry in the Process-Wide Open File List (PWOFL) which points to the SW Open File List entry and an identifier FILE2 **(this entry is a structure)**.
      3. Increment the SWOFL entry's number of reference.
      4. Initialize the PW Open File entry's Current Pointer to zero.
      5. Return the FILE2 descriptor.
   4. Return the FILE2 descriptor.

### 9. Close2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the FILE2 descriptor is valid. (Id 10: this repeats in another function)
   2. Find the file in the Process-Wide Open File List (PWOFL) using the FILE2 identifier. (Id 2: this repeats in another function)
   3. Guard temporarily the pointer to the SWOFL entry.
   4. Remove the entry from the PWOFL. **Careful not to break the list.**
   5. Decrement the SWOFL entry. If it reaches zero, remove it from the list. **Careful not to break the list.**
   6. Return Success.
   
### 10. Read2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the Buffer pointer is not NULL and Size is not negative. (Id 3: this repeats in another function)
   2. Check if the FILE2 descriptor is valid. (Id 10: this repeats in another function)
   3. Locate the file in the Process-Wide Open File List (PWOFL) using the FILE2 identifier. (Id 2: this repeats in another function)
   4. Calculate if the "File Size - Current Position < Size". This indicates that the read reached the end of the file. If so, maintain "File Size - Current Position" in a variable to be returned.
   5. Feed the Buffer through a for loop, reading sector by sector. **Be careful to do this correctly and using the indirection pointers accordingly.**  
      **Consider creating a separate function to do this.**
   6. Update the Current Position of the PWOFL entry.
   7. Return the number of bytes read.

### 11. Write2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the Buffer pointer is not NULL and Size is not negative. (Id 3: this repeats in another function)
   2. Check if the FILE2 descriptor is valid. (Id 10: this repeats in another function)
   3. Check if any new Block is going to be needed. **If needed, be careful to allocate the Blocks through the existing functions and point to the correct index entry.**
   4. Locate the file in the Process-Wide Open File List (PWOFL) using the FILE2 identifier. (Id 2: this repeats in another function)
   5. Consume the Buffer through a for loop, writing sector by sector. **Be careful to do this correctly and using the indirection pointers accordingly.**  
      **Important: if the Current Position + Size exceed the Maximum File Size, writes only Maximum File Size - Current Position.**  
      **Consider using a function to write a given number of bytes/sectors/blocks given a position, a buffer and a size.**  
      **This may need to load the given Block' sector into a buffer, write into the buffer and then write the buffer to memory.**  
         **This depends on the Current Pointer's position.**
   6. Return the number of bytes successfully written.

### 12. Opendir2:
   0. Only continues if a partition is mounted and THE Directory isn't already open.
   1. Create a (**the only**) structure for consulting THE Directory. Current Entry points to the first entry in THE Directory.
   2. Load THE Directory i-node and initialize THEDIR struct.
   3. Initialize System-Wide Open File List (probably use the list API of the first assignment).
   4. Initialize Process-Wide Open File List.
   5. Return Success.
   
### 13. Readdir2:
   0. Check if a partition is mounted and THE Directory is already open. (Id 5: this repeats in another function)
   1. Check if Current Entry is pointing to a valid dentry.
   2. Put the entry in the variable given by the user.
   3. Update the Current Entry.
   2. Return Success.
   
### 14. Closedir2:  
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if there are no open files.
   2. Close THE Directory.
   3. Deallocate Open File Lists.
   4. Return Success.
   
### 15: sln2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   2. Check if the given link name and the file name are valid. (Id 9: this repeats in another function)
   3. Check that a file with the link name doesn't exist. (Id 7: this sequence \[1,2\] repeats in another function)
   4. Check if the file which the link should point exists.
   5. Allocate an i-node and a Free Block. **Use predefined functions! Or create them!**
   6. Fill the allocated block with the string containing the file direction. **The \"/file\" string, not a pointer to the i-node!**
   7. Fill i-node with the necessary info. **Use predefined functions! Or create them!**
      1. "blocksFileSize:=0x01".
      2. "dataPtr[0]:=blockAddress".
      2. "bytesFileSize:=stringSizeInBytes". **Be carefull to add the '\0' char in this counting.**
      3. "RefCounter:=1".
   8. Create a dentry and add it to THE Directory. **Create a function for this SL dentry creation!**
   9. Return Success.

### 16: hln2:
   0. Check if a partition is mounted and THE Directory is open. (Id 5: this repeats in another function)
   1. Check if the given link name and the file name are valid. (Id 9: this repeats in another function)
   2. Check that a file with the link name doesn't exist. (Id 7: this sequence \[1,2\] repeats in another function)
   3. Check if the file which the link should point exists and is not a softlink.
   4. Fetch the file i-node ID. Load the i-node.
   5. Create a dentry and add it to THE Directory. **Create a function for this HL dentry creation!**
   6. Increment the file i-node "RefCounter".
   7. Return Success.
