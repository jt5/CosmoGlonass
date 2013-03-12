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

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;
typedef signed long s32;

class GPSClass{	
	public:
	static void begin();
	static void end();
	static u8 decode(char c);
	
	static void get_datetime(u16* year, u8* month, u8* day, u8* hour, u8* minute, u8* second, u8* ms);
	static void get_position(float* lon, float* lat);
	static float get_altitude();
	static float get_speed();
	static float get_course();
	static u8 get_sats();
	static u8 get_status();
	static float get_hdop();
	static float get_vdop();
	static float get_pdop();
	
};

extern GPSClass GPS;
extern char NMEA_BUF[128];
	
