#include <octotherm.h>


bool serverStarted = false;
HttpServer server;

void onIndex(HttpRequest &request, HttpResponse &response)
{
	response.setCache(86400, true); // It's important to use cache for better performance.
	response.sendFile("index.html");
}

void onConfiguration(HttpRequest &request, HttpResponse &response)
{

	if (request.getRequestMethod() == RequestMethod::POST)
	{
		debugf("Update config");
		// Update config
		if (request.getBody() == NULL)
		{
			debugf("NULL bodyBuf!!!\n");
			return;
		}
		else
		{
			DynamicJsonBuffer jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(request.getBody());
			root.prettyPrintTo(Serial); Serial.println(); //Uncomment it for debuging

			if (root["StaSSID"].success()) // Settings
			{
				uint8_t PrevStaEnable = ActiveConfig.StaEnable;

				ActiveConfig.StaSSID = String((const char *)root["StaSSID"]);
				ActiveConfig.StaPassword = String((const char *)root["StaPassword"]);
				ActiveConfig.StaEnable = root["StaEnable"];

				if (PrevStaEnable && ActiveConfig.StaEnable)
				{
					WifiStation.enable(true);
					WifiAccessPoint.enable(false);
					WifiStation.config(ActiveConfig.StaSSID, ActiveConfig.StaPassword);
				}
				else if (ActiveConfig.StaEnable)
				{
					WifiStation.enable(true, true);
					WifiAccessPoint.enable(false, true);
					WifiStation.config(ActiveConfig.StaSSID, ActiveConfig.StaPassword);
				}
				else
				{
					WifiStation.enable(false, true);
					WifiAccessPoint.enable(true, true);
				}
			}
			if (root["mode_switch_temp"].success()) // Settings
			{
				ActiveConfig.mode_switch_temp = root["mode_switch_temp"];
				ActiveConfig.mode_switch_temp_delta = root["mode_switch_temp_delta"];
				ActiveConfig.pump_on_delay = root["pump_on_delay"];
				ActiveConfig.pump_off_delay = root["pump_off_delay"];
				ActiveConfig.caldron_on_delay = root["caldron_on_delay"];
				ActiveConfig.room_off_delay = root["room_off_delay"];
				ActiveConfig.twvalve_temp = root["twvalve_temp"];
				ActiveConfig.twvalve_temp_delta = root["twvalve_temp_delta"];
				ActiveConfig.twvalve_step_time = root["twvalve_step_time"];
				ActiveConfig.twvalve_edge_time = root["twvalve_edge_time"];
				ActiveConfig.zone_mode = root["zone_mode"];

				tWValve->setTargetTemp(ActiveConfig.twvalve_temp);
				tWValve->setTargetTempDelta(ActiveConfig.twvalve_temp_delta);
				tWValve->setStepTime(ActiveConfig.twvalve_step_time);
				tWValve->setEdgeTime(ActiveConfig.twvalve_edge_time);

				ActiveConfig.time_zone = root["time_zone"];

				ActiveConfig.sensorUrl = String((const char *)root["sensorUrl"]);
				ActiveConfig.sensor1Url = String((const char *)root["sensor1Url"]);
				ActiveConfig.sensor2Url = String((const char *)root["sensor2Url"]);
				ActiveConfig.sensor3Url = String((const char *)root["sensor3Url"]);
				ActiveConfig.switchUrl = String((const char *)root["switchUrl"]);
				ActiveConfig.switch1Url = String((const char *)root["switch1Url"]);
				ActiveConfig.switch2Url = String((const char *)root["switch2Url"]);
			}
//			if (root["sensorUrl"].success())
//			{
//				ActiveConfig.sensorUrl = String((const char *)root["sensorUrl"]);
//				system_restart();
//			}
		}
		saveConfig(ActiveConfig);
	}
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile("config.html");
	}
}

void onConfiguration_json(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["StaSSID"] = ActiveConfig.StaSSID;
	json["StaPassword"] = ActiveConfig.StaPassword;
	json["StaEnable"] = ActiveConfig.StaEnable;

	json["time_zone"] = ActiveConfig.time_zone;

	json["sensorUrl"] = ActiveConfig.sensorUrl;
	json["sensor1Url"] = ActiveConfig.sensor1Url;
	json["sensor2Url"] = ActiveConfig.sensor2Url;
	json["sensor3Url"] = ActiveConfig.sensor3Url;
	json["switchUrl"] = ActiveConfig.switchUrl;
	json["switch1Url"] = ActiveConfig.switch1Url;
	json["switch2Url"] = ActiveConfig.switch2Url;

	json["mode_switch_temp"] = ActiveConfig.mode_switch_temp;
	json["mode_switch_temp_delta"] = ActiveConfig.mode_switch_temp_delta;
	json["pump_on_delay"] = ActiveConfig.pump_on_delay;
	json["pump_off_delay"] = ActiveConfig.pump_off_delay;
	json["caldron_on_delay"] = ActiveConfig.caldron_on_delay;
	json["room_off_delay"] = ActiveConfig.room_off_delay;
//	json["start_minutes"] = ActiveConfig.start_minutes;
//	json["stop_minutes"] = ActiveConfig.stop_minutes;
//	json["cycle_duration"] = ActiveConfig.cycle_duration;
//	json["cycle_interval"] = ActiveConfig.cycle_interval;
	json["twvalve_temp"] = ActiveConfig.twvalve_temp;
	json["twvalve_temp_delta"] = ActiveConfig.twvalve_temp_delta;
	json["twvalve_step_time"] = ActiveConfig.twvalve_step_time;
	json["twvalve_edge_time"] = ActiveConfig.twvalve_edge_time;
	json["zone_mode"] = ActiveConfig.zone_mode;

	response.sendJsonObject(stream);
}
void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
	{
//		response.forbidden();
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

void onAJAXGetState(HttpRequest &request, HttpResponse &response)
{
	JsonObjectStream* stream = new JsonObjectStream();
	JsonObject& json = stream->getRoot();

	json["counter"] = counter;
	json["tank_temp"] = localTempSensors->getTemp(1); //show _mode_curr_temp here
	json["tank_healthy"] = localTempSensors->isValid(1);
	json["valve_temp"] = localTempSensors->getTemp(2); //show valve temp here
	json["valve_healthy"] = localTempSensors->isValid(2);
	json["mode"] = HSystem._mode;
	String date_time_str = SystemClock.getSystemTimeString();
	json["date_time"] = date_time_str;

	response.sendJsonObject(stream);
}


void onStateJson(HttpRequest &request, HttpResponse &response)
{
	uint8_t currThermostat = request.getQueryParameter("thermostat").toInt();
	thermostat[currThermostat]->onStateCfg(request,response);
}

void onScheduleJson(HttpRequest &request, HttpResponse &response)
{
	uint8_t currThermostat = request.getQueryParameter("thermostat").toInt();
	thermostat[currThermostat]->onScheduleCfg(request,response);
}

void onThermostatsJson(HttpRequest &request, HttpResponse &response)
{
	StaticJsonBuffer<thermostatsJsonBufSize> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	for (uint t=0; t < maxThermostats; t++)
	{
		root[(String)t] = thermostat[t]->getName();

	}
	char buf[scheduleFileBufSize];
	root.printTo(buf, sizeof(buf));

	response.setHeader("Access-Control-Allow-Origin", "*");
	response.setContentType(ContentType::JSON);
	response.sendString(buf);


}
void startWebServer()
{
	if (serverStarted) return;

	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/config", onConfiguration);
	server.addPath("/config.json", onConfiguration_json);
	server.addPath("/state", onAJAXGetState);
	server.addPath("/state.json", onStateJson);
	server.addPath("/schedule.json", onScheduleJson);
	server.addPath("/thermostats.json", onThermostatsJson);
	server.setDefaultHandler(onFile);
	serverStarted = true;

	if (WifiStation.isEnabled())
		debugf("STA: %s", WifiStation.getIP().toString().c_str());
	if (WifiAccessPoint.isEnabled())
		debugf("AP: %s", WifiAccessPoint.getIP().toString().c_str());
}
