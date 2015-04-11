#define WIN32_LEAN_AND_MEAN

#include "windows.h"

struct Base
{

};

enum ubs_protocol_bytes {
	PREAMBLE1 = 0xb5,
	PREAMBLE2 = 0x62,
	CLASS_NAV = 0x01,
	CLASS_ACK = 0x05,
	CLASS_CFG = 0x06,
	CLASS_MON = 0x0A,
	MSG_ACK_NACK = 0x00,
	MSG_ACK_ACK = 0x01,
	MSG_POSLLH = 0x2,
	MSG_STATUS = 0x3,
	MSG_SOL = 0x6,
	MSG_VELNED = 0x12,
	MSG_CFG_PRT = 0x00,
	MSG_CFG_RATE = 0x08,
	MSG_CFG_SET_RATE = 0x01,
	MSG_CFG_NAV_SETTINGS = 0x24,
	MSG_CFG_SBAS = 0x16,
	MSG_MON_HW = 0x09,
	MSG_MON_HW2 = 0x0B
};
enum ubs_nav_fix_type {
	FIX_NONE = 0,
	FIX_DEAD_RECKONING = 1,
	FIX_2D = 2,
	FIX_3D = 3,
	FIX_GPS_DEAD_RECKONING = 4,
	FIX_TIME = 5
};
enum ubx_nav_status_bits {
	NAV_STATUS_FIX_VALID = 1
};


#pragma pack(push,1)
struct POSLLH
{
	//unsigned char header[2];
	//	unsigned char ID[2];
	//	unsigned char length[2];
	ULONG TOW;
	LONG lon;
	LONG lat;
	LONG height;
	LONG hSML;
	ULONG hAcc;
	ULONG vAcc;
};

BYTE data_velned[] = {

	0xEE, 0x6B, 0x92, 0x04,
	0xA6, 0xFE, 0xFF, 0xFF,
	0x1C, 0xFF, 0xFF, 0xFF,
	0x3F, 0x00, 0x00, 0x00,
	0xA3, 0x01, 0x00, 0x00,
	0x9E, 0x01, 0x00, 0x00,
	0xAD, 0x88, 0x4B, 0x01,
	0x6E, 0x00, 0x00, 0x00,
	0x4D, 0x2B, 0x14, 0x00,
	//0xDE, 0x26

};

struct VELNED: Base
{
	ULONG iTOW;
	LONG velN;
	LONG velE;
	LONG velD;
	ULONG speed;
	ULONG gSpeed;
	LONG heading;
	ULONG sAcc;
	ULONG cAcc;
};

struct SOL: Base
{
	ULONG iTOW;
	LONG fTOW;
	SHORT week;
	BYTE gpsFix;
	BYTE flags;
	LONG ecefX;
	LONG ecefY;
	LONG ecefZ;
	ULONG pAcc;
	LONG ecefVX;
	LONG ecefVY;
	LONG ecefVZ;
	ULONG sAcc;
	USHORT pDOP;
	BYTE reserved1;
	BYTE numSV;
	ULONG reserved2;

};

//struct STATUS
//{
//	ULONG	iTOW;
//	BYTE	gpsFix;
//	BYTE	flags;
//	BYTE	fixStat;
//	BYTE	flags2;
//	ULONG	ttff;
//	ULONG	msss;
//
//};

struct  STATUS: Base {
	ULONG time;                                  // GPS msToW
	BYTE  fix_type;
	BYTE  fix_status;
	BYTE  differential_status;
	BYTE  res;
	ULONG time_to_first_fix;
	ULONG uptime;
};


class msg
{
public:
	int type;
	VELNED velned;
	SOL sol;
	POSLLH posllh;
};

#pragma pack(pop)

void csum(BYTE * Buffer, int N)
{

	unsigned char CK_A = 0, CK_B = 0;

	for (int I = 0; I<N; I++)
	{
		CK_A = CK_A + Buffer[I];
		CK_B = CK_B + CK_A;
	}

	printf("%x %x\n", CK_A, CK_B);

}

//tow:			76704750
//vel north:		-3.46
//vel east:		-2.28
//vel down:		.63
//speed 3d:		4.19
//ground speed:	4.14
//heading 2d:		217.27
//vel 3d:			1.10
//Heading:		13.22




BYTE data_STATUS[] =
{
	//	B5 62 01 03 10 00 
	0xCA, 0x71, 0x92, 0x04,
	0x03,
	0xDD,
	0x00,
	0x00,
	0xF5, 0x0B, 0x00, 0x00,
	0xEB, 0xDD, 0x00, 0x00
	//8D B7                        
};

BYTE data_SOL[] = {

	0xE8, 0x6C, 0x92, 0x04,
	0xF9, 0x50, 0x01, 0x00,
	0x2F, 0x07, 0x03, 0xDD,
	0x06, 0xE0, 0xD1, 0x07,
	0x0A, 0x77, 0x64, 0xE4,
	0x18, 0x18, 0xDB, 0x18,
	0xA4, 0x02, 0x00, 0x00,
	0x29, 0xFF, 0xFF, 0xFF,
	0x2D, 0xFF, 0xFF, 0xFF,
	0x20, 0xFF, 0xFF, 0xFF,
	0x65, 0x00, 0x00, 0x00,
	0xC4, 0x00, 0x02, 0x07,
	0x24, 0x67, 0x01, 0x00
	//00,86       
};



unsigned char data_llh[] = {// 0xB5, 0x62, 
	//	0x01, 0x02, 0x1c, 0,
	//tow
	0x3e, 0x35, 0x92, 0x04,
	//long
	0x41, 0x40, 0xc8, 0xd3,

	0x25, 0x0B, 0x7E, 0x18, //lat
	0xBC, 0x32, 0x01, 0x00, //heigt
	0xA8, 0xB8, 0x01, 0x00, //hSML
	0x2F, 0x08, 0x00, 0x00, //hAcc
	0x8F, 0x15, 0x00, 0x00 //vAc

};

#define NAV_POSLLH_STRING "{\"NAV_POSLLH\":\n{\"TOW\":%d,\n\"lat\":%d,\n\"long\":%d,\n" \
	"\"height\":%d,\n\"hSML\":%d,\n\"hAcc\":%d,\n\"vAcc\":%d\n}},"


#define NAV_VELNED_STRING "{\"NAV_VELNED\":{\n"\
	"\"iTOW\":%d,\n" \
	"\"velN\":%d,\n" \
	"\"velE\":%d,\n" \
	"\"velD\":%d,\n" \
	"\"gSpeed\":%d,\n" \
	"\"speed\":%d,\n" \
	"\"cAcc\":%d,\n" \
	"\"sAcc\":%d,\n" \
	"\"heading\":%d}},\n"

#define NAV_STATUS_STRING \
	"{\"NAV_STATUS\":\n{" \
	"\"time:\":%d,\n"\
	"\"fix_type\":%d,\n"\
	"\"fix_status\":%d,\n"\
	"\"differential_status\":%d,\n"\
	"\"res\":%d,\n"\
	"\"time_to_first_fix\":%d,\n"\
	"\"uptime\":%d}},\n"\

#define NAV_SOL_STRING \
	"{\"NAV_SOL\":\n{\"iTOW\":%d,\n\"fTOW\":%d,\n\"week\":%d,\n\"gpsFix\":%d,\n\"flags\":%d,\n" \
	"\"ecefX\":%d,\n\"ecefY\":%d,\n\"ecefZ\":%d,\n\"pAcc\":%d,\n\"ecefVX\":%d,\n\"ecefVY\":%d,\n"\
	"\"ecefVZ\":%d,\n\"sAcc\":%d,\n\"pDOP\":%d,\n\"numSV\":%d}},\n"




DWORD pubThread(LPVOID lpdwThreadParam);