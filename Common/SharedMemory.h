
/*
 * @file	SharedMemory.h/.cpp
 * @brief	Enables shared memory using MPI.
 * @details	One processor must run the shm-server, it handles requests from other processors.
 *			Processors must Lock a block of memory before they can get/set the values.
 *			Processors must Unlock a block of memory after it is no longer needed.
 * @author	Matt Drage
 * @date	03/01/2013
 */

#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

void RunSharedMemoryServer(int numBlocks, int blockSize);
void LockSharedMemory(int server, int id);
bool UnlockSharedMemory(int server, int id);
bool GetSharedMemory(int server, int id, char *data, int dataSize);
bool SetSharedMemory(int server, int id, char *data, int dataSize);
void KillSharedMemoryServer(int server);

// Class to simplify above functions
// Remembers server index, block size, last used block id, and last used buffer
class SharedMemory
{
	public:
		// Constructor
		SharedMemory(int server, int blockSize);
	
		// Call to init the values of a block to those in the buffer
		void Init(int id, char *buffer);
	
		// Call to lock & load a block into the buffer specified
		void Obtain(int id, char *buffer);
	
		// Call to send the modified values in the buffer specified
		// in the last call to Obtain() to the shm-server
		// Also unlocks the block
		void Release();
	
		// Kill the shm-server
		void Terminate();
	
	private:
		int m_server;
		int m_blockSize;
		char *m_buffer;
		int m_blockId;
};

#endif
