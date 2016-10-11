#ifndef INCLUDE_CONFIGURATION_H_
#define INCLUDE_CONFIGURATION_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>


const char THERM_CONFIG_FILE[] = ".therm.conf"; // leading point for security reasons :)

struct ThermConfig
{
	ThermConfig()
	{
		StaEnable = 1; //Enable WIFI Client
		sensorUrl = "http://192.168.1.200/temperature.json?sensor=0";
		sensor1Url = "http://192.168.1.201/temperature.json?sensor=0";
		sensor2Url = "http://192.168.1.202/temperature.json?sensor=0";
		sensor3Url = "http://192.168.1.203/temperature.json?sensor=0";
		switchUrl = "http://192.168.1.204/set_state";
		switch1Url = "http://192.168.1.205/set_state";
		switch2Url = "http://192.168.1.206/set_state";
//		sensorUrl = "http://10.2.113.114/state";

		//HeatControl defaults
		mode_switch_temp = 25; //C degree 60 original
		mode_switch_temp_delta = 1; //C dgree
		pump_on_delay = 10; //seconds
		pump_off_delay = 10; //seconds
		caldron_on_delay = 10; //seconds
		room_off_delay = 15; //seconds
//		start_minutes = 480; // minutes since 0:00 to start WHPump cycles
//		stop_minutes = 1380; // minutes since 0:00 to stop WHPump cycles
//		cycle_duration = 5; // Duration of pump stay turning on in minutes
//		cycle_interval = 60; // Interval of turning HWPump On for a while in minutes
		time_zone = 3; // Time zone offset from GMT
		twvalve_temp = 23;
		twvalve_temp_delta = 0.5;
		twvalve_step_time = 10;
		twvalve_edge_time = 130;
		zone_mode = 4; //WARMY from termostat.h
	}

	String StaSSID;
	String StaPassword;
	uint8_t StaEnable;
	//OctoTherm settings
	String sensorUrl;
	String sensor1Url;
	String sensor2Url;
	String sensor3Url;
	String switchUrl;
	String switch1Url;
	String switch2Url;
	// HeatControl settings
	float mode_switch_temp;
	float mode_switch_temp_delta;
	uint16_t pump_on_delay;
	uint16_t pump_off_delay;
	uint16_t caldron_on_delay;
	uint16_t room_off_delay;
//	uint16_t start_minutes;
//	uint16_t stop_minutes;
//	uint8_t cycle_duration;
//	uint8_t cycle_interval;
	uint8_t time_zone;
	float twvalve_temp;
	float twvalve_temp_delta;
	uint16_t twvalve_step_time;
	uint16_t twvalve_edge_time;
	uint8_t zone_mode;
};

ThermConfig loadConfig();
void saveConfig(ThermConfig& cfg);

extern ThermConfig ActiveConfig;

#endif /* INCLUDE_CONFIGURATION_H_ */
