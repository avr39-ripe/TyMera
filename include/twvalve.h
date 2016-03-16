/*
 * twvalve.h
 *
 *  Created on: 14 марта 2016
 *      Author: shurik
 */

#ifndef INCLUDE_TWVALVE_H_
#define INCLUDE_TWVALVE_H_
#include <SmingCore/SmingCore.h>
#include <tempsensors.h>
#include <heatcontrol.h>
//#include <octotherm.h>

namespace TWValveDirection
{
	const uint8_t STOP=0;
	const uint8_t WARM=1;
	const uint8_t COLD=2;
}

class TWValve
{
public:
	TWValve(TempSensors &tempSensors, uint8_t sensorId, uint8_t warmPin, uint8_t coldPin, uint16_t refresh = 30);
	void start();
	void stop();
	void setTargetTemp(float targetTemp) { _targetTemp = targetTemp; };
	float getTargetTemp() { return _targetTemp; };
	void setTargetTempDelta(float targetTempDelta) { _targetTempDelta = targetTempDelta; };
	float getTargetTempDelta() { return _targetTempDelta; };
	void setStepTime(uint16_t stepTime) { _valveStepTime = stepTime; };
	uint16_t getStepTime() { return _valveStepTime; };
	void setEdgeTime(uint16_t edgeTime) { _valveEdgeTime = edgeTime; };
	uint16_t getEdgeTime() { return _valveEdgeTime; };
private:
	uint16_t _refresh; // thermostat update interval in SECONDS!!!
	Timer _refreshTimer; // timer for thermostat update
	TempSensors *_tempSensors; //TempSensor after 3-way valve which measure target temp.
	uint8_t	_sensorId;
	float _targetTemp = 40; //Target temp after 3-way valve
	float _targetTempDelta = 1.5; // Target temp delta
	uint8_t _warmPin; // pin to move valve warm-side
	uint8_t _coldPin; // pin to move valve cold-side
	uint8_t _valveDirection; // direction to turn valve WARM or COLD or STOP
	uint16_t _valveStepTime = 10; // step to turn valve on, wait, turn valve off in SECONDS
	uint16_t _valveEdgeTime = 130; // time valve goes from HOT edge to COLD edge in SECONDS
	Timer _stopperTimer; // timer for turn valve on, wait(timer!), turn valve off
	uint8_t _consumers = 0;
	void _valveTurn(uint8_t direction, uint16_t moveTime);
	void _thermostat();
	void _valveStop();
};




#endif /* INCLUDE_TWVALVE_H_ */
