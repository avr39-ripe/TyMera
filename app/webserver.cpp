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
			debugf("NULL bodyBuf");
			return;
		}
		else
		{
			StaticJsonBuffer<ConfigJsonBufferSize> jsonBuffer;
			JsonObject& root = jsonBuffer.parseObject(request.getBody());
			root.prettyPrintTo(Serial); //Uncomment it for debuging

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
			if (root["sensorUrl"].success())
			{
				ActiveConfig.sensorUrl = String((const char *)root["sensorUrl"]);
				system_restart();
			}
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
	json["sensorUrl"] = ActiveConfig.sensorUrl;

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
	json["temperature"] = tempSensor->getTemp();
	json["healthy"] = tempSensor->isHealthy();

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
