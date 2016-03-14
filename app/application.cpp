#include <user_config.h>
#include <octotherm.h>

Timer counterTimer;
void counter_loop();
unsigned long counter = 0;
OneWire ds(onewire_pin);
TempSensorsOW *localTempSensors;
TempSensorsHttp *tempSensors;
Thermostat *thermostat[maxThermostats];
SwitchHttp officeSwitch("http://192.168.31.204/set_state");

NtpClient ntpClient("pool.ntp.org", 300);

void STADisconnect(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason);
void STAGotIP(IPAddress ip, IPAddress mask, IPAddress gateway);

//HeatControl Addition - START
Timer SPITimer;
Timer HSystemTimer;
String _date_time_str = "";
void HSystem_loop();

#ifdef MCP23S17 //use MCP23S17 SPI_loop
MCP inputchip(1, mcp23s17_cs);             // Instantiate an object called "inputchip" on an MCP23S17 device at  address 1 = 0b00000001 and CS pin = GPIO16
MCP outputchip(0, mcp23s17_cs);            // Instantiate an object called "outputchip" on an MCP23S17 device at address 0 = 0b00000010 and CS pin = GPIO16
#endif
//HeatControl Addition - STOP

void onOfficeStateChange(bool state)
{
	Serial.printf("Office state changed to %s!\n", state ? "true" : "false");
	officeSwitch.setState(state);
}

void initialWifiConfig()
{
	struct softap_config apconfig;
	if(wifi_softap_get_config_default(&apconfig))
	{
		if (os_strncmp((const char *)apconfig.ssid, (const char *)"TyTherm", 32) != 0)
		{
			WifiAccessPoint.config("TyTherm", "20040229", AUTH_WPA2_PSK);

		}
		else
			Serial.printf("AccessPoint already configured.\n");
	}
	else
		Serial.println("AP NOT Started! - Get config failed!");

	if (WifiStation.getSSID().length() == 0)
	{
		WifiStation.config(WIFI_SSID, WIFI_PWD);
		WifiStation.enable(true, true);
		WifiAccessPoint.enable(false, true);
	}
	else
		Serial.printf("Station already configured.\n");
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false);
	Serial.commandProcessing(false);

	Serial.printf("COMPILE-IN SSID: %s, PASSWORD: %s\n", WIFI_SSID, WIFI_PWD);

	SystemClock.setTimeZone(2);

	//SET higher CPU freq & disable wifi sleep
	system_update_cpu_freq(SYS_CPU_160MHZ);
	wifi_set_sleep_type(NONE_SLEEP_T);

	initialWifiConfig(); //One-time WIFI setup

	ActiveConfig = loadConfig();
	ds.begin();

	localTempSensors = new TempSensorsOW(ds, 4000);
	localTempSensors->addSensor((String)"289D143E000000DB");
	localTempSensors->addSensor((String)"28E31D3E000000A3");
	localTempSensors->addSensor((String)"2897DD3D0000004D");
	localTempSensors->start();

	tempSensors = new TempSensorsHttp(4000);
	tempSensors->addSensor("http://192.168.31.238/temperature.json?sensor=0");
	tempSensors->addSensor("http://192.168.31.238/temperature.json?sensor=1");
	tempSensors->addSensor("http://192.168.31.238/temperature.json?sensor=2");
	tempSensors->addSensor("http://192.168.31.130/temperature.json?sensor=0");
	thermostat[0] = new Thermostat(*localTempSensors,0,"Office", 4000);
	thermostat[0]->onStateChange(onStateChangeDelegate(&SwitchHttp::setState, &officeSwitch));
	thermostat[1] = new Thermostat(*localTempSensors,1,"Kitchen", 4000);
	thermostat[2] = new Thermostat(*localTempSensors,2,"Hall", 4000);
	thermostat[3] = new Thermostat(*tempSensors,3,"Bedroom", 4000);

	for(uint8_t i = 0; i< 7; i++)
	{
		for (auto _thermostat: thermostat)
		{
			_thermostat->_schedule[i][0].start = 0;
			_thermostat->_schedule[i][0].targetTemp = 800;
			_thermostat->_schedule[i][1].start = 360;
			_thermostat->_schedule[i][1].targetTemp = 1800;
			_thermostat->_schedule[i][2].start = 540;
			_thermostat->_schedule[i][2].targetTemp = 1200;
			_thermostat->_schedule[i][3].start = 720;
			_thermostat->_schedule[i][3].targetTemp = 1500;
			_thermostat->_schedule[i][4].start = 1020;
			_thermostat->_schedule[i][4].targetTemp = 1800;
			_thermostat->_schedule[i][5].start = 1320;
			_thermostat->_schedule[i][5].targetTemp = 800;

			_thermostat->loadStateCfg();
			_thermostat->loadScheduleBinCfg();
		}
	}

	auto tWValve = new TWValve(*localTempSensors,2,4,5);
	tWValve->setTargetTemp(23);
	tWValve->setTargetTempDelta(0.5);
	tWValve->start();

	WifiEvents.onStationDisconnect(STADisconnect);
	WifiEvents.onStationGotIP(STAGotIP);

	startWebServer();

	counterTimer.initializeMs(1000, counter_loop).start();

	//HeatControl Addition - START
	//SPI_loop init
	#ifdef MCP23S17 //use MCP23S17 SPI_loop
		inputchip.begin();
		outputchip.begin();
		inputchip.pinMode(0xFFFF);     // Use word-write mode to set all of the pins on inputchip to be inputs
		inputchip.pullupMode(0xFFFF);  // Use word-write mode to Turn on the internal pull-up resistors.
	//	inputchip.inputInvert(0x0000); // Use word-write mode to invert the inputs so that logic 0 is read as HIGH
		outputchip.pinMode(0x0000);    // Use word-write mode to Set all of the pins on outputchip to be outputs

		Serial.println("---===MCP23S17 VERSION===---");
	#endif

	#ifndef MCP23S17
		pinMode(reg_in_latch, OUTPUT);
		pinMode(reg_out_latch, OUTPUT);
	#endif

	SPITimer.initializeMs(200, SPI_loop).start();

	HSystemTimer.initializeMs(2000, HSystem_loop).start();
	HSystem._hwpump->cycle();

//	// I2C bus config and init
//    Wire.pins(scl_pin, sda_pin);
//    Wire.begin();
//
//    //Initial setup & sync from DSRTC system clock
//    SystemClock.setTimeZone(ActiveConfig.time_zone);
//    SystemClock.setTime(DSRTC.get(), eTZ_UTC);
	//HeatControl Addition - STOP
}

void HSystem_loop()
{
	HSystem.check();
}

void counter_loop()
{
	counter++;
//	counter = wifi_station_get_rssi();
//	Serial.printf("RSSI: %d\n", counter);
}
void STADisconnect(String ssid, uint8_t ssid_len, uint8_t bssid[6], uint8_t reason)
{
	Serial.printf("DELEGATE DISCONNECT - SSID: %s, REASON: %d\n", ssid.c_str(), reason);

	if (!WifiAccessPoint.isEnabled())
	{
		Serial.println("Starting OWN AP DELEGATE");
		WifiStation.disconnect();
		WifiAccessPoint.enable(true);
		WifiStation.connect();
	}
}

void STAGotIP(IPAddress ip, IPAddress mask, IPAddress gateway)
{
	Serial.printf("DELEGATE GOTIP - IP: %s, MASK: %s, GW: %s\n", ip.toString().c_str(),
																mask.toString().c_str(),
																gateway.toString().c_str());

	if (WifiAccessPoint.isEnabled())
	{
		WifiAccessPoint.enable(false);
	}

	ntpClient.requestTime();
	tempSensors->start();
	for (auto _thermostat: thermostat)
		_thermostat->start();
	officeSwitch.start();
}
