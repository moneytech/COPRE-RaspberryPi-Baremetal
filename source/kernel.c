/****************************************************
* kernel.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#define ON 1
#define OFF 0

extern void OkLedOn(void);
extern void OkLedOff(void);

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern unsigned int GetTickCount(void);

int Entrypoint(void) {
	unsigned int ledState, ledCount;
	ledState = OFF;
	ledCount = GetTickCount();

	while(1) {
		// Switch state of OK/ACT LED every 1 second
		if(GetTickCount() > ledCount + 1000000) {
			if(ledState == ON) {
				OkLedOff();
				ledState = OFF;
			} else {
				OkLedOn();
				ledState = ON;
			}
			ledCount = GetTickCount() + 1000000;
		}
	}

	return 0;
}
