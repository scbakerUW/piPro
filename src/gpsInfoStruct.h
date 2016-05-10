/* gpsInfoStruct.h */

#ifndef GPSINFOSTRUCT_H_
#define GPSINFOSTRUCT_H_



struct GPS_DATA{
	char* rawtime;  //
	char* hours;
	char* minutes;
	char* seconds;
	float latitude;
	char latDir[2];
	float longitude;
	char longDir[2];
	char fix[2];
	int num_sats;
	char speed[12];
	char* rawdate;
	char* month;
	char* day;
	char* year;

};

#endif