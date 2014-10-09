#ifndef RTP_STRUCT_H
#define RTP_STRUCT_H

#include <stdint.h>
#include <sys/types.h>
/*
  RTP packet header bit offset 	0-1 	2 	3 	4-7 	8 	9-15 	16-31
  0 	Version 	P 	X 	CC 	M 	PT 	Sequence Number
  32 	Timestamp
  64 	SSRC identifier
  96 	CSRC identifiers
  ...
  96+32×CC 	Profile-specific extension header ID 	Extension header length
  128+32×CC 	Extension header
  ...
  
 */
//#define RTP_BIG_ENDIAN
struct RTPHeader
{
#ifdef RTP_BIG_ENDIAN
	uint8_t version:2;
	uint8_t padding:1;
	uint8_t extension:1;
	uint8_t csrccount:4;
	
	uint8_t marker:1;
	uint8_t payloadtype:7;
#else // little endian
	uint8_t csrccount:4;
	uint8_t extension:1;
	uint8_t padding:1;
	uint8_t version:2;
	
	uint8_t payloadtype:7;
	uint8_t marker:1;
#endif // RTP_BIG_ENDIAN

	uint16_t sequencenumber;
	uint32_t timestamp;
	uint32_t ssrc;	
};

struct RTPExtensionHeader
{
	uint16_t extid;
	uint16_t length;
};

struct RTPSourceIdentifier
{
	uint32_t ssrc;
};

struct stRTPPacket
{
	RTPHeader header;
	unsigned char data[128];	// 42
};

inline int u16_to_BigEndian(uint16_t num)
{
	return ((num & 0xff00) >> 8) | ((num & 0x00ff) << 8);
}

inline int u32_to_BigEndian(uint32_t num)
{
	return (((num & 0xff)       << 24) |
			((num & 0xff00)     << 8 ) |
			((num & 0xff0000)   >> 8 ) |
			((num & 0xff000000) >> 24) );
}

#endif	// RTP_STRUCT_H
