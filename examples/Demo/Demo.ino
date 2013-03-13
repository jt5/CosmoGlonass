#include <SoftwareSerial.h>
#include <GPS.h>

SoftwareSerial ss(3, 2);

float latitude;
float longitude;
float altitude;
float speed;
float pdop;
float hdop;
float vdop;
unsigned char sats_GPS;
unsigned char sats_GLONASS;

unsigned int year;
byte month, day, hour, minute, second, ms;

void setup() {
	Serial.begin(115200);
	Serial.println("Testing GPS/GLONASS library...");
	ss.begin(9600);
}

void loop() {
	while (ss.available()) {    
		char c = ss.read();
		if (GPS.decode(c)) {

			GPS.get_position(&longitude, &latitude);
			altitude = GPS.get_altitude();
			pdop = GPS.get_pdop();
			hdop = GPS.get_hdop();
			vdop = GPS.get_vdop();
			GPS.get_datetime(&year, &month, &day, &hour, &minute, &second, &ms);
	  
			sats_GPS = GPS.get_GPS_sats();
			sats_GLONASS = GPS.get_GLONASS_sats();
			altitude = GPS.get_altitude();
			speed = GPS.get_speed();
      
			//Serial.println(NMEA_BUF);
      
			Serial.print("GPS status: ");
			if (GPS.get_status()) Serial.println ("OK");
			else Serial.println ("--");
      
			Serial.print("GPS Sats: ");
			Serial.println(sats_GPS);
          
			Serial.print("GLONASS Sats: ");
			Serial.println(sats_GLONASS);      
      
			Serial.print("DATE/TIME: ");      
			char sz[32];
			sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d   ",
			day, month, year, hour, minute, second);        
			Serial.println(sz);
        
			Serial.print("LAT: ");
			Serial.println(latitude,6);
			Serial.print("LON: ");  
			Serial.println(longitude,6);
      
			Serial.print("Speed: ");
			Serial.println(speed,2);      
      
			Serial.print("Altitude: ");
			Serial.println(altitude,2);
      
			Serial.print("HDOP: ");  
			Serial.println(hdop);
			Serial.print("VDOP: ");    
			Serial.println(vdop);
			Serial.print("PDOP: ");    
			Serial.println(pdop);
      
			delay(2000);
		}
	}
}