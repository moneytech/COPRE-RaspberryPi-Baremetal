/****************************************************
* mailbox.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/types.h"

#define MAILBOX_READ 0x2000B880
#define MAILBOX_STATUS 0x2000B898
#define MAILBOX_WRITE 0x2000B8A0

/*
* MailboxWrite:
* Write a message to the mailbox
* void* input: The data to write to the mailbox
* u32 mailbox: The mailbox channel to write to
*
* Returns: (u32) 1 for success, 0 for failure
*/
u32 MailboxWrite(void* input, u32 mailbox)
{
	//this checks that the lowest 4 bits of the input are 0 ( this will be where the mailbox address "channel" will be)
	if ((*((u32*)input) & 0xF) > 0)
	{
		return 0;
	}
	
	//This checks if the mailbox address is correct, 0-15
	if (mailbox > 15)
	{
		return 0;
	}
	
	u32 ready; ready = 0;

	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		u32 mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);
		// if the top bit is 0 then you are ready to write.
		if ((mailStatus & 0x80000000) == 0)
		{
			ready = 1;
		}

	}

	//add the mailbox "chanel" onto the input
	unsigned int finalMessage; finalMessage = *((u32*)input) + mailbox;

	//store the value at the write address ( address, value)
	PutUInt32(MAILBOX_WRITE, finalMessage);

	return 1;
}

/*
* MailboxRead:
* Read from the mailbox
* u32 mailbox: The mailbox channel to read from
*
* Returns: (u32) The message read from the mailbox
*/
u32 MailboxRead(u32 mailbox)
{
	// this checks to make sure we have a valid 0-15 mailbox
	if (mailbox > 15)
	{
		// returns 1 as the GPU will return 0 at the end of this function if everything is ok
		return 0;
	}

	u32 newMail;
	u32 ready; ready = 0;

	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		u32 mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);

		// if the 30th bit is 0 then you are ready to read.
		if ((mailStatus & 0x40000000) == 0)
		{
			//read the next item in the mailbox
			newMail = GetUInt32(MAILBOX_READ);

			//check to make sure the channel of the message we have just read is the one we want
			if ((newMail & 0xF) == mailbox)
			{
				// break the whle loop 
				ready = 1;
			}

		}

	}

	//returns the top 28 bits of the mail ( delete the channel off of the mail )
	return (newMail & 0xfffffff0);
}