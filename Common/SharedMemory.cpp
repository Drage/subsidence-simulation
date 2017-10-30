
#include "SharedMemory.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cassert>
#include <unistd.h>

const char SHM_SUCCESS = 's';
const char SHM_FAIL = 'f';
const char SHM_TERMINATE = 't';
const char SHM_LOCK = 'l';
const char SHM_UNLOCK = 'u';
const char SHM_GET = 'g';
const char SHM_SET = 's';

void SendFail(int processorIndex)
{
	char fail = SHM_FAIL;
	MPI_Send(&fail, 1, MPI_CHAR, processorIndex, 0, MPI_COMM_WORLD);
}

void SendSuccess(int processorIndex)
{
	char success = SHM_SUCCESS;
	MPI_Send(&success, 1, MPI_CHAR, processorIndex, 0, MPI_COMM_WORLD);
}

void RunSharedMemoryServer(int numBlocks, int blockSize)
{
	// Create shared memory
	char *data = new char[numBlocks * blockSize];
	memset(data, 0, numBlocks * blockSize);
	
	// Create mutexes
	int *lock = new int[numBlocks];
	for (int i = 0; i < numBlocks; i++)
		lock[i] = -1;
	
	// Vars for server loop
	int bufferSize = blockSize + 8;
	char *buffer = new char[bufferSize];
	MPI_Status status;
	int source;
	int index;
	bool terminate = false;
	
	// Respond to requests
	while (!terminate)
	{
		// Wait for request message
		MPI_Recv(buffer, bufferSize, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
		source = status.MPI_SOURCE;
		
		// Handle request
		switch (buffer[0])
		{
			// Terminate server process, free shared memory
			case SHM_TERMINATE:
				terminate = true;
				break;
				
			// Lock mutex
			case SHM_LOCK:
				sscanf(buffer + 1, "%d", &index);
				if (lock[index] == -1)
				{
					lock[index] = source;
					SendSuccess(source);
				}
				else
					SendFail(source);
				break;
				
			// Unlock mutex
			case SHM_UNLOCK:
				sscanf(buffer + 1, "%d", &index);
				if (lock[index] == source)
				{
					lock[index] = -1;
					SendSuccess(source);
				}
				else
					SendFail(source);
				break;
				
			// Get a data block
			case SHM_GET:
				sscanf(buffer + 1, "%d", &index);
				if (lock[index] == source) 
				{
					SendSuccess(source);
					MPI_Send(data + index * blockSize, blockSize, MPI_CHAR, source, 0, MPI_COMM_WORLD);
				}
				else
					SendFail(source);
				break;
				
			// Set a data block
			case SHM_SET:
				sscanf(buffer + 1, "%d", &index);
				if (lock[index] == source)
				{
					memcpy(data + index * blockSize, strchr(buffer, ':') + 1, blockSize);
					SendSuccess(source);
				}
				else
					SendFail(source);
				break;
		}
	}
	
	// Clean up shared memory and mutexes
	delete[] buffer;
	delete[] data;
	delete[] lock;
}

void LockSharedMemory(int server, int id)
{
	// Create lock request (opcode + block index)
	char request[8];
	request[0] = SHM_LOCK;
	sprintf(request + 1, "%d", id);
	char reply = '\0';
	
	// Poll shm-server until lock can be obtained
	do
	{
		MPI_Send(request, 8, MPI_CHAR, server, 0, MPI_COMM_WORLD);
		MPI_Recv(&reply, 1, MPI_CHAR, server, 0, MPI_COMM_WORLD, NULL);
		
		// Data block locked by another process, wait 1sec before trying again
		if (reply == SHM_FAIL) 
			usleep(100);
		
	} while (reply == SHM_FAIL);
}

bool UnlockSharedMemory(int server, int id)
{
	// Send unlock request (opcode + block index)
	char request[8];
	request[0] = SHM_UNLOCK;
	sprintf(request + 1, "%d", id);
	MPI_Send(request, 8, MPI_CHAR, server, 0, MPI_COMM_WORLD);
	
	// Check whether unlock succeeded (must have locked first)
	char reply;
	MPI_Recv(&reply, 1, MPI_CHAR, server, 0, MPI_COMM_WORLD, NULL);
	return reply != SHM_FAIL;
}

bool GetSharedMemory(int server, int id, char *data, int dataSize)
{
	// Send get request (opcode + block index)
	char request[8];
	request[0] = SHM_GET;
	sprintf(request + 1, "%d", id);
	MPI_Send(request, 8, MPI_CHAR, server, 0, MPI_COMM_WORLD);
	
	// Check if get succeeded (must have lock first)
	char reply;
	MPI_Recv(&reply, 1, MPI_CHAR, server, 0, MPI_COMM_WORLD, NULL);
	if (reply == SHM_FAIL)
		return false;
	else
	{
		// Success - recv data
		MPI_Recv(data, dataSize, MPI_CHAR, server, 0, MPI_COMM_WORLD, NULL);
		return true;
	}
}

bool SetSharedMemory(int server, int id, char *data, int dataSize)
{
	// Send set request (opcode + block index + : + data)
	int bufferSize = dataSize + 8;
	char *request = new char[bufferSize];
	request[0] = SHM_SET;
	sprintf(request + 1, "%d:", id);
	memcpy(strchr(request, ':') + 1, data, dataSize);	
	MPI_Send(request, bufferSize, MPI_CHAR, server, 0, MPI_COMM_WORLD);
	
	// Check if set succeeded (must have lock first)
	char reply;
	MPI_Recv(&reply, 1, MPI_CHAR, server, 0, MPI_COMM_WORLD, NULL);
	return reply != SHM_FAIL;
}

void KillSharedMemoryServer(int server)
{
	// Send terminate request
	char request = SHM_TERMINATE;
	MPI_Send(&request, 1, MPI_CHAR, server, 0, MPI_COMM_WORLD);
}

SharedMemory::SharedMemory(int server, int blockSize)
{
	char *m_buffer = NULL;
	int m_blockId = -1;
	m_server = server;
	m_blockSize = blockSize;
}

void SharedMemory::Init(int id, char *buffer)
{
	assert(m_server != -1);
	assert(buffer != NULL);
	
	LockSharedMemory(m_server, id);
	SetSharedMemory(m_server, id, buffer, m_blockSize);
	UnlockSharedMemory(m_server, id);
}

void SharedMemory::Obtain(int id, char *buffer)
{
	assert(m_server != -1);
	assert(buffer != NULL);
	
	m_blockId = id;
	m_buffer = buffer;
	
	LockSharedMemory(m_server, m_blockId);
	GetSharedMemory(m_server, m_blockId, m_buffer, m_blockSize);
}

void SharedMemory::Release()
{
	assert(m_blockId != -1);
	assert(m_buffer != NULL);
	
	SetSharedMemory(m_server, m_blockId, m_buffer, m_blockSize);
	UnlockSharedMemory(m_server, m_blockId);
	
	m_blockId = -1;
	m_buffer = NULL;
}

void SharedMemory::Terminate()
{
	KillSharedMemoryServer(m_server);
}
