/****************************************************
* mailbox.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/types.h"

u32 MailboxWrite(void* input, u32 mailbox);
u32 MailboxRead(u32 mailbox);