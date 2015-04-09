#include "stdafx.h"
// ubloxtest.cpp : Defines the entry point for the console application.
//
#include "ubloxtest.h"


using namespace std;


#if 0
tow : 76690.750
  longt : -74.1851071
	  lat : 41.0913573

			B5 62 01 02 1C 00 
			3E 35 92 04 //TOW
			41 40 C8 D3 //lon
			25 0B 7E 18 //lat
			BC 32 01 00 //heigt
			A8 B8 01 00 //hSML
			2F 08 00 00 //hAcc
			8F 15 00 00 //vAcc
			35 76 //check

			construct.SLInt32("long").parse("\x41\x40\xc8\xd3")
			- 741851071

			>> > construct.ULInt32("long").parse("\x3e\x35\x92\x04")
			76690750 #works


#endif 

const char * fname = R"(C:\Users\Greg Brill\Documents\run1.ubx)";


/*
	  _last_pos_time = _buffer.posllh.time;
	  state.location.lng = _buffer.posllh.longitude;
	  state.location.lat = _buffer.posllh.latitude;
	  state.location.alt = _buffer.posllh.altitude_msl / 10;
	  state.status = next_fix;
	  _new_position = true;
#if UBLOX_FAKE_3DLOCK
	  state.location.lng = 1491652300L;
	  state.location.lat = -353632610L;
	  state.location.alt = 58400;
#endif
	  state.horizontal_accuracy = _buffer.posllh.horizontal_accuracy*1.0e-3f;
	  state.vertical_accuracy = _buffer.posllh.vertical_accuracy*1.0e-3f;
	  state.have_horizontal_accuracy = true;
	  state.have_vertical_accuracy = true;
*/

void dumpPOSLLH(struct POSLLH *  llh)
{
	printf(NAV_POSLLH_STRING,
		llh->TOW,
		llh->lat,
		llh->lon,
		llh->height,
		llh->hSML,
		llh->hAcc,
		llh->vAcc


		);


}

//case MSG_VELNED:
//	Debug("MSG_VELNED");
//	_last_vel_time = _buffer.velned.time;
//	state.ground_speed = _buffer.velned.speed_2d*0.01f;          // m/s
//	state.ground_course_cd = _buffer.velned.heading_2d / 1000;       // Heading 2D deg * 100000 rescaled to deg * 100
//	state.have_vertical_velocity = true;
//	state.velocity.x = _buffer.velned.ned_north * 0.01f;
//	state.velocity.y = _buffer.velned.ned_east * 0.01f;
//	state.velocity.z = _buffer.velned.ned_down * 0.01f;
//	state.have_speed_accuracy = true;
//	state.speed_accuracy = _buffer.velned.speed_accuracy*0.01f;
//	_new_speed = true;
//	break;
//
void dumpVELNED(struct VELNED * velned)
{

	printf(NAV_VELNED_STRING,
		velned->iTOW,
		velned->velN,
		velned->velE,
		velned->velD,
		velned->gSpeed,
		velned->speed,
		velned->cAcc,
		velned->sAcc,
		velned->heading);


}

void dumpSTATUS(struct STATUS * status)
{
	
	printf(NAV_STATUS_STRING,

		status->time,
		status->fix_type,
		status->fix_status,
		status->differential_status,
		status->res,
		status->time_to_first_fix,
		status->uptime
		);

}

void dumpSOL(struct SOL * sol)
{

	printf(NAV_SOL_STRING,

		sol->iTOW,
		sol->fTOW,
		sol->week,
		sol->gpsFix,
		sol->flags,
		sol->ecefX,
		sol->ecefY,
		sol->ecefZ,
		sol->pAcc,
		sol->ecefVX,
		sol->ecefVY,
		sol->ecefVZ,
		sol->sAcc,
		sol->pDOP,
		sol->numSV);



}

void toggleOut(char * fname)
{
	if (fname)
		freopen(fname, "w", stdout);
	else
	freopen("CON", "w", stdout);



}

void readit()
{
	unsigned char x[2];
	unsigned char msgtype[2];//class / ID
	unsigned short length;
	struct POSLLH llh;
	struct VELNED velned;
	struct SOL sol;
	struct STATUS status;
	

	streampos begin, end, current;
	ifstream myfile(fname, ios::binary);
	printf("{\"records\":[\n");
	while (!myfile.eof())
	{
		myfile.read((char*)x, 2);

		current = myfile.tellg();
		if (x[0] == 0xb5 && x[1] == 0x62)
		{
			//read the class and msg type
			myfile.read((char*)msgtype, 2);
			myfile.read((char*)&length, 2);
			current = myfile.tellg();
			switch (msgtype[0])// class
			{
			case 0x1://NAV

				switch (msgtype[1])
				{
				case 0x2: //LLH
					//printf("LLH begin at:%d, reading:%d\n",(int)current, length);
					myfile.read((char*)&llh, length);//+2 is the checksum
					dumpPOSLLH(&llh);
					break;

				case 0x3://status
					myfile.read((char*)&status, length);//+2 is the checksum
					dumpSTATUS(&status);
					break;

				case 0x12: //velned
					//	printf("status\n");
					//	printf("velned begin at:%d, reading:%d\n", (int)current, length);
					myfile.read((char*)&velned, length);//+2 is the checksum
					dumpVELNED(&velned);
					break;

				case 0x6: //SOL
					myfile.read((char*)&sol, length);//+2 is the checksum
					dumpSOL(&sol);

					break;
				}



				break;
			default:				
				//normally this is TIM-TP (0x0D 0x01)
				;// printf("\n--->Not A Nav:msg type %x %x\n", msgtype[0], msgtype[1]);


			}

			//printf("found at:%d lenth:%d\n", (int)current, length);

		}

		//if ((int)current > 400)
		//{
		//	printf("\n{\"EOF\":0}]}\n");
		//	return;
		//}

	}
		printf("\n{\"EOF\":0}]}\n");


	//printf("%d %d", x[0], x[1]);
	//cout <<(unsigned short)x[0]<<" "<<(unsigned short)x[1]<<":"<< myfile.tellg()<<endl;
	//myfile.read(x, 2);
	//cout <<(unsigned short) x[0] <<" "<<(unsigned short) x[1] << ":" << myfile.tellg() << endl;



}


int _tmain(int argc, _TCHAR* argv[])
{
	//struct POSLLH s;
	////TOW SHOULD be 76700.250
	//struct VELNED v;
	//struct SOL sol;
	//struct STATUS status;

	////csum(data_llh, sizeof(data));

	//memcpy(&s, (const void *)data_llh, sizeof(data_llh));
	//memcpy(&v, (const void *)data_velned, sizeof(data_velned));
	//memcpy(&sol, (const void *)data_SOL, sizeof(data_SOL));
	//memcpy(&status, (const void *)data_STATUS, sizeof(data_STATUS));

	toggleOut("c:\\temp\\test2.json");
	readit();
	return 0;

	//long should be -74.1851071
	//printf("%d %d\n", s.TOW, s.lon);

	//unsigned char test[4];
	//unsigned char test2[4];

	//test[3] = 4;
	//test[2] = 0x92;
	//test[1] = 0x35;
	//test[0] = 0x3e;

	//test2[3] = 0x3e;
	//test2[2] = 0x35;
	//test2[1] = 0x92;
	//test2[0] = 0x4;

	////only first is correct
	//printf("%d %d", *(signed int*)test, *(signed int*)test2);

	return 0;
}
