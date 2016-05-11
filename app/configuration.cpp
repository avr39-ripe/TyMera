#include <octotherm.h>

ThermConfig ActiveConfig;

ThermConfig loadConfig()
{
	DynamicJsonBuffer jsonBuffer;
	ThermConfig cfg;
	if (fileExist(THERM_CONFIG_FILE))
	{
		int size = fileGetSize(THERM_CONFIG_FILE);
		char* jsonString = new char[size + 1];
		fileGetContent(THERM_CONFIG_FILE, jsonString, size + 1);
		JsonObject& root = jsonBuffer.parseObject(jsonString);

		JsonObject& network = root["network"];
		cfg.StaSSID = String((const char*)network["StaSSID"]);
		cfg.StaPassword = String((const char*)network["StaPassword"]);
		cfg.StaEnable = network["StaEnable"];

		JsonObject& settings = root["settings"];
		cfg.sensorUrl = String((const char*)settings["sensorUrl"]);
		cfg.sensor1Url = String((const char*)settings["sensor1Url"]);
		cfg.sensor2Url = String((const char*)settings["sensor2Url"]);
		cfg.sensor3Url = String((const char*)settings["sensor3Url"]);
		cfg.switchUrl = String((const char*)settings["switchUrl"]);
		cfg.switch1Url = String((const char*)settings["switch1Url"]);
		cfg.switch2Url = String((const char*)settings["switch2Url"]);
		//HeatControl
		cfg.mode_switch_temp = settings["mode_switch_temp"];
		cfg.mode_switch_temp_delta = settings["mode_switch_temp_delta"];
		cfg.pump_on_delay = settings["pump_on_delay"];
		cfg.pump_off_delay = settings["pump_off_delay"];
		cfg.caldron_on_delay = settings["caldron_on_delay"];
		cfg.room_off_delay = settings["room_off_delay"];
		cfg.start_minutes = settings["start_minutes"];
		cfg.stop_minutes = settings["stop_minutes"];
		cfg.cycle_duration = settings["cycle_duration"];
		cfg.cycle_interval = settings["cycle_interval"];
		cfg.time_zone = settings["time_zone"];
		cfg.twvalve_temp = settings["twvalve_temp"];
		cfg.twvalve_temp_delta = settings["twvalve_temp_delta"];
		cfg.twvalve_step_time = settings["twvalve_step_time"];
		cfg.twvalve_edge_time = settings["twvalve_edge_time"];
		cfg.zone_mode = settings["zone_mode"];
		delete[] jsonString;
	}
	else
	{
		//Factory defaults if no config file present
		cfg.StaSSID = WIFI_SSID;
		cfg.StaPassword = WIFI_PWD;
	}
	return cfg;
}

void saveConfig(ThermConfig& cfg)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	JsonObject& network = jsonBuffer.createObject();
	root["network"] = network;
	network["StaSSID"] = cfg.StaSSID.c_str();
	network["StaPassword"] = cfg.StaPassword.c_str();
	network["StaEnable"] = cfg.StaEnable;

	JsonObject& settings = jsonBuffer.createObject();
	root["settings"] = settings;
	settings["sensorUrl"] = cfg.sensorUrl;
	settings["sensor1Url"] = cfg.sensor1Url;
	settings["sensor2Url"] = cfg.sensor2Url;
	settings["sensor3Url"] = cfg.sensor3Url;
	settings["switchUrl"] = cfg.switchUrl;
	settings["switch1Url"] = cfg.switch1Url;
	settings["switch2Url"] = cfg.switch2Url;
	//HeatControl
	settings["mode_switch_temp"] = cfg.mode_switch_temp;
	settings["mode_switch_temp_delta"] = cfg.mode_switch_temp_delta;
	settings["pump_on_delay"] = cfg.pump_on_delay;
	settings["pump_off_delay"] = cfg.pump_off_delay;
	settings["caldron_on_delay"] = cfg.caldron_on_delay;
	settings["room_off_delay"] = cfg.room_off_delay;
	settings["start_minutes"] = cfg.start_minutes;
	settings["stop_minutes"] = cfg.stop_minutes;
	settings["cycle_duration"] = cfg.cycle_duration;
	settings["cycle_interval"] = cfg.cycle_interval;
	settings["time_zone"] = cfg.time_zone;
	settings["twvalve_temp"] = cfg.twvalve_temp;
	settings["twvalve_temp_delta"] = cfg.twvalve_temp_delta;
	settings["twvalve_step_time"] = cfg.twvalve_step_time;
	settings["twvalve_edge_time"] = cfg.twvalve_edge_time;
	settings["zone_mode"] = cfg.zone_mode;

	String buf;
	root.prettyPrintTo(buf);
	fileSetContent(THERM_CONFIG_FILE, buf);
}


