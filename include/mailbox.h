/****************************************************
* mailbox.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/types.h"

u32 MailboxWrite(u32 input, u32 channel);
u32 MailboxRead(u32 channel);