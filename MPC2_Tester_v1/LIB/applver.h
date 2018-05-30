#ifndef APP_VER_INCLUDED
#define APP_VER_INCLUDED 1



#define VERSION(version_high, version_low) ((((unsigned long)version_high) << 16) | (version_low))
#define VERSION_HIGH(version) (((version) >> 16) & 0xffff)
#define VERSION_LOW(version) ((version) & 0xffff)

extern const unsigned long ApplProgVersion;
extern const unsigned char DbmsString[8];

//Onderstaande constante alleen met veelvouden van 4 verhogen  
#pragma memory=constseg(ROMVER) 
extern const unsigned long CompRomVersion[12];
extern const unsigned long CompDbmsVersion[8];
#pragma memory = default
extern unsigned short RomVersion;
extern unsigned short ApplVersion;
extern void SetVersion(void);


#endif
