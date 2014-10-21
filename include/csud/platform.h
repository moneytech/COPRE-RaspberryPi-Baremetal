/******************************************************************************
*	platform/platform.h
*	 by Alex Chadwick
*
*	A light weight implementation of the USB protocol stack fit for a simple
*	driver.
*
*	platform/platform.h contains definitions pertaining to the platform that
*	the system will run on.
******************************************************************************/

typedef unsigned int u32;

/**
	\brief Allocates memory of specified length. 

	Allocates memory of length at least length to the driver and returns the 
	address. If MEM_INTERNAL_MANAGER is defined, then all platforms must
	provide an implementation which calls MemoryReserve. Can return NULL on
	error.
*/
void* MemoryAllocate(u32 length);
/**
	\brief Deallocates memory of specified address, previously allocated by
	MemoryAllocate. 

	Deallocates memory of the specified address that was previously allocated
	by MemoryAllocate. If MEM_INTERNAL_MANAGER is defined, then all platforms
	must provide an implementation. Calling with an address not received from
	MemoryAllocate produces undefined results.
*/
void MemoryDeallocate(void* address);
/**
	\brief Notifies the system of memory usage. 

	Notifies the parent system of an unavoidable memory usage. This is 
	typically used for memory mapped IO systems, in which certain addresses
	have special meaning. It is up to the parent system to implement whatever
	must be done. The return value should be a virtual address that maps to 
	the requested physical address, or NULL on error. If MEM_NO_RESERVE is 
	defined, a dummy implementation is created.
*/
void* MemoryReserve(u32 length, void* physicalAddress);
/**
	\brief Copies chunks of memory. 

	Copies length bytes from source to destinatoin. If either source or 
	destination are null, should not copy anything.
*/
void MemoryCopy(void* destination, void* source, u32 length);

/**
	\brief Notifies the user of progress. 

	Notifies the parent system of progress loading the driver. Messages may be 
	displayed to a semi-technically competant user.
*/
void LogPrint(const char* message, u32 messageLength);
/**
	\brief Notifies the user of progress. 

	Prints our a formatted string. Uses all the formtting options in printf. 
	Implemented in platform.c, by calling LogPrint. Messages truncated to 160
	characters.
*/
void LogPrintF(char* format, u32 formatLength, ...);

#define LOG(x) (LogPrint(x, sizeof(x)))
#define LOGL(x, len) (LogPrint(x, len))
#define LOGF(x, ...) (LogPrintF(x, sizeof(x), __VA_ARGS__))
#define LOGFL(x, len, ...) (LogPrintF(x, len, __VA_ARGS__))

#define LOG_DEBUG(x) 
#define LOG_DEBUGL(x, len) 
#define LOG_DEBUGF(x, ...) 
#define LOG_DEBUGFL(x, len, ...)