#ifndef ROM_VER_INCLUDED
#define ROM_VER_INCLUDED 1


#define VERSION(version_high, version_low) ((((unsigned long)version_high) << 16) | (version_low))
#define VERSION_HIGH(version) (((version) >> 16) & 0xffff)
#define VERSION_LOW(version) ((version) & 0xffff)


extern const unsigned long RomProgVersion;
extern const unsigned long CheckSum_Rom;

extern non_banked void StartRom(void);
extern non_banked void RomReset(void);
extern non_banked void ClearChkSmAppl(void);


#endif
