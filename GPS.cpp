/*
Name:		GPS/GLONASS library for Arduino
Version:	1.0
Created:	07.03.2013
Updated:	12.03.2013
Programmer:	Erezeev A.
Production:	JT5.RU
Source:		https://github.com/jt5/CosmoGlonass

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/pgmspace.h>
#include <avr/io.h>
#include "GPS.h"
#include <string.h>

char NMEA_BUF[128];

typedef struct 
{
}
tGPRMC;

void GPSClass::begin()
{
}

void GPSClass::end()
{
}

const char _xxGSV[] PROGMEM = "GSV"; 
const char _GPRMC[] PROGMEM = "GPRMC";
const char _GPGGA[] PROGMEM = "GPGGA";
const char _GNGSA[] PROGMEM = "GNGSA";

enum
{
	xxGSV =1,
	GPRMC =3,
	GPGGA =4,
	GNGSA =5,
};



u8 NMEA_ID (char* pSrc)
{
	if		(strstr_P(pSrc, _xxGSV)) return xxGSV;
	else if (strstr_P(pSrc, _GPRMC)) return GPRMC;
	else if (strstr_P(pSrc, _GPGGA)) return GPGGA;
	else if (strstr_P(pSrc, _GNGSA)) return GNGSA;
	else return 0;
}

u8 NMEA_type;

u8 isdigit(char c) { return c >= '0' && c <= '9'; }
	
long atol(const char *str)
{
	long ret = 0;
	while (isdigit(*str))
	ret = 10 * ret + *str++ - '0';
	return ret;
}

u32 parse_decimal(char *p)
{
	u8 isneg = *p == '-';
	if (isneg) ++p;
	u32 ret = 100UL * atol(p);
	while (isdigit(*p)) ++p;
	if (*p == '.')
	{
		if (isdigit(p[1]))
		{
			ret += 10 * (p[1] - '0');
			if (isdigit(p[2]))
			ret += p[2] - '0';
		}
	}
	return isneg ? -ret : ret;
}

u32 GPS_time;
u8 GPS_valid;
float GPS_f_latitude;
float GPS_f_longitude;

u32 GPS_speed;
u32 GPS_course;
u32 GPS_date;

u8  GPS_GP_sats;
u8  GPS_GL_sats;
u32 GPS_hdop;
u32 GPS_vdop;
u32 GPS_pdop;
s32 GPS_altitude;

char* sel_words (char* p, u8 n)
{
	while (n--)
	{	
		while (*p != ',')
		{
			p++;
		}	
	}
	return p + 1;
}

char* shift_words (char* p)
{
		while (*p != ',')
		{
			p++;
		}
	return p + 1;
}

u32 parse_degrees(char* Src)
{
	char *p = Src;
	u32 left = atol(Src);
	u32 tenk_minutes = (left % 100UL) * 10000UL;
	
	for (p=Src; isdigit(*p); ++p);
	if (*p == '.')
	{
		unsigned long mult = 1000;
		while (isdigit(*++p))
		{
			tenk_minutes += mult * (*p - '0');
			mult /= 10;
		}
	}
	return (left / 100) * 100000 + tenk_minutes / 6;
}

float parse_lat(char* p)
{
	u16 DD;
	u16  MM;
	u16 MMMM = 0x0000;
	
	float Dec;

	DD = ((u16) (*p) - '0') * 10 + (*(p+1)) - '0';
	MM = ((u16) (*(p+2)) - '0') * 10 + (*(p+3)) - '0';

	MMMM = ( (u16) (*(p+5)) - '0') * 1000 + ( (u16) (*(p+6)) - '0') * 100 + ( (u16) (*(p+7)) - '0') * 10 +( (u16) (*(p+8)) - '0');	
	
	Dec = (float) DD + (float (MM) + float (MMMM) / 10000.0) / 60.0;		
	return (Dec);
}


float parse_lon(char* p)
{
	u16 DDD;
	u8  MM;
	u16 MMMM = 0x0000;
	
	float Dec;
	
	DDD = ((u16) (*p) - '0') * 100 + ((u16) (*(p+1)) - '0') * 10 + (*(p+2)) - '0';	
	MM = ((u16) (*(p+3)) - '0') * 10 + (*(p+4)) - '0';
	MMMM = ( (u16) (*(p+6)) - '0') * 1000 + ( (u16) (*(p+7)) - '0') * 100 + ( (u16) (*(p+8)) - '0') * 10 +( (u16) (*(p+9)) - '0');
	
	Dec = (float) DDD + (float (MM) + float (MMMM) / 10000.0) / 60.0;
	return (Dec);
}

u8 process_GPRMC( char * pSrc)
{
	pSrc = shift_words(pSrc);
	GPS_time = parse_decimal (pSrc);
	pSrc = shift_words(pSrc);
	if (*pSrc == 'A') GPS_valid = 1;
	else GPS_valid = 0;
	
	pSrc = shift_words(pSrc);
	GPS_f_latitude = 	parse_lat (pSrc);
	pSrc = shift_words(pSrc);
	if (*pSrc == 'S') GPS_f_latitude = -GPS_f_latitude;
	
	pSrc = shift_words(pSrc);
	GPS_f_longitude = 	parse_lon (pSrc);
	pSrc = shift_words(pSrc);
	if (*pSrc == 'W') GPS_f_longitude = -GPS_f_longitude;
	
	pSrc = shift_words(pSrc);
	GPS_speed = parse_decimal (pSrc);
	pSrc = shift_words(pSrc);
	GPS_course = parse_decimal (pSrc);
	pSrc = shift_words(pSrc);
	GPS_date = atol(pSrc);
}

u8 process_GPGGA(char * pSrc)
{
	pSrc = shift_words(pSrc);
	GPS_time = parse_decimal (pSrc);
	
	pSrc = shift_words(pSrc);
	GPS_f_latitude = 	parse_lat (pSrc);
	
	pSrc = shift_words(pSrc);
	if (*pSrc == 'S') GPS_f_latitude = -GPS_f_latitude;
	
	pSrc = shift_words(pSrc);	
	GPS_f_longitude = 	parse_lon (pSrc);
	
	pSrc = shift_words(pSrc);
	if (*pSrc == 'W') GPS_f_longitude = -GPS_f_longitude;
	
	pSrc = shift_words(pSrc);
	if (atol(pSrc)>0) GPS_valid = 1;
	else GPS_valid = 0;
	
	pSrc = shift_words(pSrc);
	
	pSrc = shift_words(pSrc);	
	GPS_hdop = parse_decimal (pSrc);
	
	pSrc = shift_words(pSrc);	
	GPS_altitude = parse_decimal(pSrc);
			
}

u8 process_GNGSA(char * pSrc)
{
	for (u8 i = 0; i < 15; i++) pSrc = shift_words(pSrc);
	GPS_pdop = parse_decimal (pSrc);
	
	pSrc = shift_words(pSrc);
	GPS_hdop = parse_decimal (pSrc);	
	
	pSrc = shift_words(pSrc);
	GPS_vdop = parse_decimal (pSrc);	
}	
	
	
u8 process_xxGSV(char * pSrc)
{
	u8 Sat_type, sats;
	if (pSrc[1] == 'P')
	{
		Sat_type = 1; // GPS sats
	}	
	else if (pSrc[1] == 'L')
	{
		Sat_type = 2; // GLONASS sats
	}
	pSrc = shift_words(pSrc);
	pSrc = shift_words(pSrc);
	pSrc = shift_words(pSrc);
	sats = (u8) atol (pSrc);
	if (Sat_type == 1)		GPS_GP_sats = sats;	
	else if (Sat_type == 2)	GPS_GL_sats = sats;
}

u8 process_message()
{	
	u8 message_ID = NMEA_ID(NMEA_BUF);
	if (message_ID == GPRMC)
	{
		process_GPRMC(NMEA_BUF);
		return 1;		
	}
	else if (message_ID == GPGGA)
	{
		process_GPGGA(NMEA_BUF);
		return 1;
	}
	else if (message_ID == GNGSA)
	{
		process_GNGSA(NMEA_BUF);
		return 1;
	}
	else if (message_ID == xxGSV)
	{
		process_xxGSV(NMEA_BUF);
		return 1;
	}
	else return 0;
}

u8 atoh (u8 i)
{
	if (isdigit (i)) return ( i - '0' );
	else if ((i >= 'A') && (i <= 'F')) return (i - 'A' + 10);
	else
	{
		
	}
}

u8 GPSClass :: decode(char c)
{
static u8 rx_i;
static u8 word_num;
static u8 rx_now, crc_now;
static u8 checksum, _checksum;

	if (rx_now)
	{
		if (c!= '*')
		{
			NMEA_BUF[rx_i++] = c;
			checksum ^= c;
			if (c == ',') word_num++;
		}			 
		else
		{
			NMEA_BUF[rx_i] = 0x00;
			rx_now = 0;
			crc_now = 1;
			rx_i = 0;
		}					

	}
	else if (crc_now)
	{
		if (rx_i == 0)
		{
			 _checksum = atoh(c) * 16;
			 rx_i++;
		}			 
		else if (rx_i == 1)
		{
			 _checksum += atoh(c);
			 crc_now = 0;
			 rx_i = 0;
			 if (checksum == _checksum) 
			 {
				 if (process_message()) return 1;
			 }				 
			 else return 0;
		}			 
		
	}
	else
	{
		if (c == '$')
		{
				rx_i = 0;
				word_num = 0;
				rx_now = 1;
				checksum = 0x00;			
		}

	}
	return 0;		
}

void GPSClass :: get_datetime(u16* year, u8* month, u8* day, u8* hour, u8* minute, u8* second, u8* ms)
{
	  if (year)
	  {
		  *year = GPS_date % 100;
		  *year += *year > 80 ? 1900 : 2000;
	  }
	  if (month) *month = (GPS_date / 100) % 100;
	  if (day) *day = GPS_date / 10000;
	  if (hour) *hour = GPS_time / 1000000;
	  if (minute) *minute = (GPS_time / 10000) % 100;
	  if (second) *second = (GPS_time / 100) % 100;
	  if (ms) *ms = GPS_time % 100;
}

void GPSClass :: get_position(float* lon, float* lat)
{
	*lon = GPS_f_longitude;
	*lat = GPS_f_latitude;	
}

float GPSClass :: get_altitude()
{
	return (float) GPS_altitude / 100.0;
}

float GPSClass :: get_speed()
{
	return (float) GPS_speed / 100.0;	
}

float GPSClass :: get_course()
{
	return (float) GPS_course / 100.0;	
}

u8 GPSClass :: get_GPS_sats()
{
	return GPS_GP_sats;
}

u8 GPSClass :: get_GLONASS_sats()
{
	return GPS_GL_sats;
}

u8 GPSClass :: get_status()
{
	if (GPS_valid)	return 1;
	else return 0;
}

float GPSClass :: get_hdop()
{
	return (float) GPS_hdop / 100.0;
}


float GPSClass :: get_vdop()
{
	return (float) GPS_vdop / 100.0;
}

float GPSClass :: get_pdop()
{
	return (float) GPS_pdop / 100.0;
}