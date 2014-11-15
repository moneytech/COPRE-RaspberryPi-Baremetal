/****************************************************
* mailbox.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/types.h"
#include "../include/debug.h"

static volatile u32* MAILBOX_READ = (u32*)0x2000B880;
static volatile u32* MAILBOX_STATUS = (u32*)0x2000B898;
static volatile u32* MAILBOX_WRITE = (u32*)0x2000B8A0;

#define MAILBOX_FULL 0x80000000
#define MAILBOX_EMPTY 0x40000000

/*
* Data memory barrier
* No memory access after the DMB can run until all memory accesses before it
* have completed
*/
#define MEMORY_BARRIER() asm volatile \
	("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0) )

/*
* MailboxWrite:
* Write a message to the mailbox
* void* input: The data address to write to the mailbox (16 bit aligned)
* u32 mailbox: The mailbox channel to write to
*
* Returns: (u32) 1 for success, 0 for failure
*/
u32 MailboxWrite(u32 input, u32 channel)
{	
	// This checks if the mailbox address is correct, 0-15
	if (channel > 15)
	{
		return 0;
	}
	
	u32 ready; ready = 0;

	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		//u32 mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);
		
		// if the top bit is 0 then you are ready to write.
		if ((*MAILBOX_STATUS & MAILBOX_FULL) == 0)
		{
			ready = 1;
		}

	}

	MEMORY_BARRIER();
	*MAILBOX_WRITE = (input | channel);
	return 1;
}

/*
* MailboxRead:
* Read from the mailbox
* u32 mailbox: The mailbox channel to read from
*
* Returns: (u32) The message read from the mailbox
*/
u32 MailboxRead(u32 channel)
{
	u32 newMail;
	u32 ready; ready = 0;

	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		//u32 mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);

		// if the 30th bit is 0 then you are ready to read.
		if ((*MAILBOX_STATUS & MAILBOX_EMPTY) == 0)
		{
			//read the next item in the mailbox
			//newMail = GetUInt32(MAILBOX_READ);

			MEMORY_BARRIER();
			newMail = *MAILBOX_READ;
			MEMORY_BARRIER();

			//check to make sure the channel of the message we have just read is the one we want
			if ((newMail & 0xF) == channel)
			{
				// break the whle loop 
				ready = 1;
			}

		}

	}

	//returns the top 28 bits of the mail ( delete the channel off of the mail )
	return (newMail & 0xfffffff0);
}