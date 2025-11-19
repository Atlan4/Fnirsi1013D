
//#define DS3231_W 0xD0     // 0x4E	//write
//#define DS3231_R 0xD1     // 0x4F 	//read

#define RTC_DEVICE_ADDR    0xD0

#include "types.h"

void iniDS3231(void);   //not implement

void readtime(void);

void settime(void);
void setdate(void);

void hourUp (void);
void hourDown (void);
void minuteUp (void);
void minuteDown (void);

void dayUp (void);
void dayDown (void);
void monthUp (void);
void monthDown (void);
void yearUp (void);
void yearDown (void);

void modnametoRTC ( int8 *d, int8 *s );
void modnameClearRTC ( int8 *d );
void readnameRTC (void);
void decodethumbnailfilename (int8 *s );

DWORD get_fattime (void);
uint8 readtemperature(void);

