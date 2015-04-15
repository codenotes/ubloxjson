#include "stdafx.h"
// ubloxtest.cpp : Defines the entry point for the console application.
//
#include "ubloxtest.h"
#include "memstream.h"

using namespace std;


std::deque<std::shared_ptr<msg>> gMsgs;

bool gVerbose = true;


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

const char * fname = R"(C:\Users\gbrill\Documents\run1.ubx)";


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
	if (gVerbose)
		printf(NAV_POSLLH_STRING,
			llh->TOW,
			llh->lat,
			llh->lon,
			llh->height,
			llh->hSML,
			llh->hAcc,
			llh->vAcc
			);

	msg * m = new msg;
	m->msgClass = NAV_CLASS;
	m->type = POSLLH_TYPE;
	m->posllh= *llh;
	std::shared_ptr<msg> p(m);
	gMsgs.push_back(p);

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
	if (gVerbose)
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

	msg * m = new msg;

	m->msgClass = NAV_CLASS;
	m->type = VELNED_TYPE;
	m->velned = *velned;
	std::shared_ptr<msg> p(m);
	gMsgs.push_back(p);

	/*VELNED * v2=new VELNED;
	*v2 = *velned;
	std::shared_ptr<Base> p(v2);
	gMsgs.push_back(p);
	*/


}

void dumpSTATUS(struct STATUS * status)
{
	
	if (gVerbose)
	printf(NAV_STATUS_STRING,

		status->time,
		status->fix_type,
		status->fix_status,
		status->differential_status,
		status->res,
		status->time_to_first_fix,
		status->uptime
		);

	msg * m = new msg;
	m->msgClass = NAV_CLASS;
	m->type = STATUS_TYPE;
	m->status = *status;
	std::shared_ptr<msg> p(m);
	gMsgs.push_back(p);


}

void dumpSOL(struct SOL * sol)
{
	if (gVerbose)
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

	msg * m = new msg;

	m->msgClass = NAV_CLASS;
	m->type = SOL_TYPE;
	m->sol = *sol;
	std::shared_ptr<msg> p(m);
	gMsgs.push_back(p);


}

void dumpTP(struct TP * tp)
{
	if (gVerbose)
	printf(TIME_TP_STRING,
		tp->towMS,
		tp->towSubMS,
		tp->qErr,
		tp->week,
		tp->flags
		);

	msg * m = new msg;
	m->msgClass = TIME_CLASS;
	m->type = TP_TYPE;
	m->tp= *tp;
	std::shared_ptr<msg> p(m);
	gMsgs.push_back(p);



}

void toggleOut(char * fname)
{
	if (fname)
		freopen(fname, "w", stdout);
	else
	freopen("CON", "w", stdout);



}

void readit(istream &myfile)
{
	unsigned char x[2];
	unsigned char msgtype[2];//class / ID
	unsigned short length;
	struct POSLLH llh;
	struct VELNED velned;
	struct SOL sol;
	struct STATUS status;
	struct TP tp;

	
	
	streampos begin, end, current;
	//ifstream myfile(fname, ios::binary);
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

				default:
					printf("{\"UNKNOWN_NAV_TYPE\":\n{\"class\":%d,\n\"type\":%d}},\n",
						msgtype[0], msgtype[1]);
					break;
				}
			case 0xd:

				switch (msgtype[1])
				{
				case 0x1:
					myfile.read((char*)&tp, TP_LENGTH);//+2 is the checksum
					dumpTP(&tp);
					break;

				}


				break;

			
			default:	
				printf("{\"UNKNOWN_CLASS\":\n{\"class\":%d,\n\"type\":%d}},\n",
					msgtype[0], msgtype[1]);

				//normally this is TIM-TP (0x0D 0x01)
				break;// printf("\n--->Not A Nav:msg type %x %x\n", msgtype[0], msgtype[1]);


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

HANDLE hSerial;

void openCOM(char * COM="COM1")
{
	hSerial = CreateFile(COM,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			//serial port does not exist. Inform user.
		}
		//some other error occurred. Inform user.
	}


	DCB dcbSerialParams = { 0 };
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) {
		//error getting state
	}
	dcbSerialParams.BaudRate = CBR_38400;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!SetCommState(hSerial, &dcbSerialParams)){
		//error setting serial port state
	}


}
#define MAX_SIZE 512

void sendCOM(BYTE msgClass, int type, int length, void * inbuff)
{

	DWORD dwBytesWritten = 0;
	
	BYTE buf[MAX_SIZE];
	BYTE b1, b2;

	buf[0] = 0xb5;
	buf[1] = 0x62;


	buf[2] = msgClass;
	buf[3] = type;
	buf[4] = length;
	buf[5] = 0;

	memcpy(buf + 6, inbuff, length);
	csum(buf + 2, 2 + 2 + length, b1, b2);
	buf[2 + 2 + 2 + length] = b1;
	buf[2 + 2 + 2 + length + 1] = b2;

	//2 start, 2 classifier, 2 size,  payload, 2 checksum = 44
	WriteFile(hSerial, buf, 2 + 2 + 2 + length + 2, &dwBytesWritten, 0);

}



void writeCOM()
{
	//char szBuff[n + 1] = { 0 };
	//DWORD dwBytesWritten = 0;
	std::shared_ptr<msg> p = gMsgs.front();
	gMsgs.pop_front();
	BYTE buf[MAX_SIZE];
	//BYTE b1, b2;
	//int size;

	buf[0] = 0xb5;
	buf[1] = 0x62;

	//fprintf(stderr,"pub:\n");

	switch (p->msgClass)
	{

	case NAV_CLASS:
		switch (p->type)
		{
		case VELNED_TYPE: //velned
			sendCOM(NAV_CLASS, VELNED_TYPE, VELNED_LENGTH, &p->velned);


			//buf[2] = 0x01;
			//buf[3] = 0x12;
			//buf[4] = VELNED_LENGTH;
			//buf[5] = 0;

			//memcpy(buf + 6, &(p->velned), VELNED_LENGTH);
			//csum(buf + 2, 2 + 2 + VELNED_LENGTH, b1, b2);
			//buf[2 + 2 + 2 + VELNED_LENGTH] = b1;
			//buf[2 + 2 + 2 + VELNED_LENGTH + 1] = b2;

			////2 start, 2 classifier, 2 size, 36 payload, 2 checksum = 44
			//WriteFile(hSerial, buf, 44, &dwBytesWritten, 0);

			////WriteFile(hSerial, data_STATUS, sizeof(data_STATUS), &dwBytesWritten, 0);

			break;

		case POSLLH_TYPE: //LLH
			sendCOM(NAV_CLASS, POSLLH_TYPE, POSLLH_LENGTH, &p->posllh);
			break;

		case STATUS_TYPE://status
			sendCOM(NAV_CLASS, STATUS_TYPE, STATUS_LENGTH, &p->status);
			break;

		case SOL_TYPE: //SOL
			sendCOM(NAV_CLASS, SOL_TYPE, SOL_LENGTH, &p->sol);
			break;


		}
		break;

	case TIME_CLASS:

		switch (p->type)
		{
		case TP_TYPE:
			sendCOM(TIME_CLASS, TP_TYPE, TP_LENGTH, &p->tp);
			break;


		}
		break;


	
	
	
	}

	//end
	//if (!WriteFile(hSerial, &m, n, &dwBytesWritten, NULL))
	//{
	//	//error occurred. Report to user.
	//}
}

DWORD pubThread(LPVOID lpdwThreadParam)
{
	while (true)
	{
		if (gMsgs.size() == 0) continue;

	
		writeCOM();
	
		Sleep(SLEEP_TIME);
	}

	return 0;
}





void hexifydump(char * HEXStr)
{
	
	std::string str(HEXStr);
//	std::string::iterator pend=
		
	str.erase(remove_if(str.begin(), str.end(), isspace), str.end());

	
	

//	cout << str << "!!\n";
	int len = str.length();


	
	char * buf = new char[len+1];
	strcpy(buf, str.c_str());
	

		int i, n;
		//char *HEXStr = "A4505D0B0F6AEDAA";
		//unsigned char tmpByte[255];


		unsigned char * tmpByte = new unsigned char[len+1];


		char backHex[255];

		for (i = 0; i < len; i++) {
			sscanf(buf+2 * i, "%2X", &n);
			tmpByte[i] = (char)n;
			
		}

		//for (i = 0; i < len; i++)
		//	sprintf(backHex + 2 * i, "%02X", tmpByte[i]);
		//
		//backHex[len] = '\0';
		//
		memstream s(tmpByte, len);
		readit(s);
	//	printf("%s\n", backHex);

		

		

		//dumpVELNED((VELNED*)(tmpByte));

		

}

void LoadGPSEmulator(char * filename)
{

	ifstream myReadFile(filename);
	std::string s, all;
	basic_string <char>::size_type look;

	char output[512];
	if (myReadFile.is_open()) 
	{
		while (!myReadFile.eof()) 
		{
			
			getline(myReadFile, s);
			//myReadFile >> output;
			//cout << output<<endl;
			
			if (s.length()>1)
				if (s[1] == '*')
					continue;

			look = s.find(":");
			if (look!= string::npos)
			{

				s = s.substr(look+1);
			
			}


			all += s; 

			//cout << s<<endl;


		}
	}
	myReadFile.close();

	hexifydump((char*)all.c_str());

}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD dwThreadId;

	//{01 12 24 22 10 92 04 09 00 00 00 0D 00 00 00 3D 00 00 00 3F 00 00 00 10 00 00 00 00 00 00 00 47 00 00 00 94 23 BB 00};
	stringstream ss;
	LoadGPSEmulator(R"(C:\Users\GBrill\Dropbox\lib\ublox\emu.txt)");
	// //removed this
	//hexifydump("B5 62 01 12 24 00 CB 14 4B 13 FC 06 00 00 00 00 00 00 00 00 00 00 FC 06 00 00 FC 06 00 00 00 00 00 00 11 01 00 00 00 51 25 02 04 33");
	return 0;


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


	
	openCOM("COM1");
//	toggleOut("c:\\temp\\junk.json");
	gVerbose=true;
	ifstream myfile(fname, ios::binary);
	readit(myfile);
	
#if 0	
	HANDLE h = CreateThread(NULL, //Choose default security
		0, //Default stack size
		(LPTHREAD_START_ROUTINE)&pubThread,
		//Routine to execute
		(LPVOID)0, //Thread parameter
		0, //Immediately run the thread
		&dwThreadId //Thread Id
		);

	WaitForSingleObject(h, INFINITE);
#endif

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
