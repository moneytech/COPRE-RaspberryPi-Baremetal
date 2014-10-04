/****************************************************
* graphics.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern void OkLedOn(void);
extern void OkLedOff(void);

#define MAILBOX_READ 0x2000B880
#define MAILBOX_STATUS 0x2000B898
#define MAILBOX_WRITE 0x2000B8A0
//#define FRAMEBUFFER_ADDRESS 0x40040020

//#define SCREEN_WIDTH 800
//#define SCREEN_HEIGHT 600

static unsigned int m_screenWidth;
static unsigned int m_screenHeight;
static unsigned int m_bitDepth;
static unsigned int m_framebufferAddress;


unsigned int MailboxWrite(unsigned int input, unsigned int mailbox)
{
	//this checks that the lowest 4 bits of the input are 0 ( this will be where the mailbox address "channel" will be)
	if ((input & 0xF) > 0)
	{
		return 0;
	}
	
	//This checks if the mailbox address is correct, 0-15
	if (mailbox > 15)
	{
		return 0;
	}
	

	unsigned int ready; ready = 0;
	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		unsigned int mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);
		// if the top bit is 0 then you are ready to write.
		if ((mailStatus & 0x80000000) == 0)
		{
			ready = 1;
		}

	}

	//add the mailbox "chanel" onto the input
	unsigned int finalMessage = input + mailbox;

	//store the value at the write address ( address, value)
	PutUInt32(MAILBOX_WRITE, finalMessage);

	return 1;
}

unsigned int MailboxRead(unsigned int mailbox)
{
	// this checks to make sure we have a valid 0-15 mailbox
	if (mailbox > 15)
	{
		// returns 1 as the GPU will return 0 at the end of this function if everything is ok
		return 0;
	}

	unsigned int newMail;

	unsigned int ready; ready = 0;
	while (!ready)
	{
		// get the status of the mailbox, shifting 16 bytes to the status address
		unsigned int mailStatus; mailStatus = GetUInt32(MAILBOX_STATUS);

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

unsigned int InitGraphics(unsigned int screenWidth, unsigned int screenHeight, unsigned int bitDepth)
{
	//set global parameters
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_bitDepth = bitDepth;


	//Set up the frame buffer info

	//physical width of the screen
	PutUInt32(0x40040000, m_screenWidth);
	//physical height of the screen
	PutUInt32(0x40040004, m_screenHeight);
	//virtual width of the screen
	PutUInt32(0x40040008, m_screenWidth);
	// virtual height of the screen
	PutUInt32(0x4004000C, m_screenHeight);
	//pitch ( number of bytes between each row of the frame buffer) set by the GPU ( set to 0 for now )
	PutUInt32(0x40040010, 0);
	//bit depth
	PutUInt32(0x40040014, m_bitDepth);
	// X offset
	PutUInt32(0x40040018, 0);
	// Y offset
	PutUInt32(0x4004001C, 0);
	// pointer returned by the GPU ( set to 0 for now )r
	PutUInt32(0x40040020, 0);
	// size of the frame buffer, set by the GPU ( set to 0 for now )
	PutUInt32(0x40040024, 0);

	// !!!! This seems to break the system, looks like we dont need to add on 0x40000000. !!!!
	// add on 0x40000000 into the value in the frame buffer to specify how to write it to the structure, this will make the GPU clear its cache so we can see the result
	//PutUInt32(0x40040000, (GetUInt32(0x40040000) + 0x40000000));

	// send the frame buffer info to channel 1
	if (MailboxWrite(0x40040000, 1) == 0)
	{
		return 0;
	}

	unsigned int frameBufferResponce;
	frameBufferResponce = MailboxRead(1);

	if (frameBufferResponce == 0)
	{
		m_framebufferAddress = GetUInt32(0x40040020);
		return 1;
	}
	return 0;
}

void RenderBackground(void)
{
	unsigned int frameAddr; frameAddr = m_framebufferAddress;
	// sets the entire screen to white
	int i, j;
	for (i = 0; i < m_screenWidth; i++)
	{
		for (j = 0; j < m_screenHeight; j++)
		{
			PutUInt32(frameAddr, 0xD3D3D3FF);
			// move the frame buffer on 4 bytes for the next pixel
			frameAddr += m_bitDepth / 8;
		}
	} 
}

void RenderPixel(unsigned int x, unsigned int y, unsigned int colour)
{
	unsigned int frameAddr; frameAddr = m_framebufferAddress;

	if (x > m_screenWidth || y > m_screenHeight)
	{
		return;
	}

	unsigned int offset;
	offset = y * m_screenWidth;
	offset += x;
	offset *= m_bitDepth / 8;

	PutUInt32(frameAddr + offset, colour);
}

void UpdateGraphics(void)
{
	RenderBackground();

	RenderPixel(401,300,0x0000000F);
	RenderPixel(402,300,0x0000000F);
	RenderPixel(403,300,0x0000000F);
	RenderPixel(404,300,0x0000000F);

	RenderPixel(401,301,0x0000000F);
	RenderPixel(402,301,0x0000000F);
	RenderPixel(403,301,0x0000000F);
	RenderPixel(404,301,0x0000000F);

	RenderPixel(401,302,0x0000000F);
	RenderPixel(402,302,0x0000000F);
	RenderPixel(403,302,0x0000000F);
	RenderPixel(404,302,0x0000000F);

	RenderPixel(401,303,0x0000000F);
	RenderPixel(402,303,0x0000000F);
	RenderPixel(403,303,0x0000000F);
	RenderPixel(404,303,0x0000000F);	
}



