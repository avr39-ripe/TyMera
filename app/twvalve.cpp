/*
 * twvalve.cpp
 *
 *  Created on: 14 марта 2016
 *      Author: shurik
 */

#include <twvalve.h>

//TWValve

TWValve::TWValve(TempSensors &tempSensors, uint8_t sensorId, uint8_t warmPin, uint8_t coldPin, uint16_t refresh)
{
	_tempSensors = &tempSensors;
	_sensorId = sensorId;
	_warmPin = warmPin;
	_coldPin = coldPin;
	_refresh = refresh;
// turn of both switches for sure
	_valveTurn(TWValveDirection::STOP, 0);
}

void TWValve::start()
{
	_thermostat();
	_refreshTimer.initializeMs(_refresh*1000, TimerDelegate(&TWValve::_thermostat, this)).start(true);
}

void TWValve::stop()
{
	_refreshTimer.stop();
	_valveTurn(TWValveDirection::COLD, _valveEdgeTime); //Default to full cold position
}

void TWValve::_valveTurn(uint8_t direction, uint16_t moveTime)
{
	_valveDirection = direction;

	switch (_valveDirection)
	{
	case TWValveDirection::WARM:
		setState(out_reg, _coldPin, false); //OFF
		setState(out_reg, _warmPin, true); //ON
		Serial.printf("TurnON WARM\n");
		break;
	case TWValveDirection::COLD:
		setState(out_reg, _warmPin, false); //OFF
		setState(out_reg, _coldPin, true); //ON
		Serial.printf("TurnON COLD\n");
		break;
	case TWValveDirection::STOP:
		setState(out_reg, _warmPin, false); //OFF
		setState(out_reg, _coldPin, false); //OFF
		Serial.printf("TurnOFF ALL\n");
		return; //DO NOT ARM TIMER!
		break;
	default:
		break;
	};
	_stopperTimer.initializeMs(moveTime*1000, TimerDelegate(&TWValve::_valveStop, this)).start(false);
}

void TWValve::_valveStop()
{
	switch (_valveDirection)
	{
	case TWValveDirection::WARM:
		setState(out_reg, _warmPin, false); //OFF
		Serial.printf("TurnOFF WARM\n");
		break;
	case TWValveDirection::COLD:
		setState(out_reg, _coldPin, false); //OFF
		Serial.printf("TurnOFF COLD\n");
		break;
	default:
		break;
	};
}

void TWValve::_thermostat()
{
	float currentTemp = _tempSensors->getTemp(_sensorId);
	//TODO: do something if temperature is invalid!!
	Serial.printf("3-Way Valve temp: "); Serial.println(currentTemp);

	//above range
	if ( currentTemp > (_targetTemp + _targetTempDelta) )
	{
		Serial.printf("Above tT+tTD = "); Serial.println(_targetTemp + _targetTempDelta);
		_valveTurn(TWValveDirection::COLD, _valveStepTime);
		return;
	}
	//below range
	if ( currentTemp < (_targetTemp - _targetTempDelta) )
	{
		Serial.printf("Below tT+tTD = "); Serial.println(_targetTemp - _targetTempDelta);
		_valveTurn(TWValveDirection::WARM, _valveStepTime);
		return;
	}
	//in range
	if ( (_targetTemp - _targetTempDelta) <= currentTemp <= (_targetTemp + _targetTempDelta) )
	{
		Serial.printf("In range!\n");
		_valveTurn(TWValveDirection::STOP, 0);
		return;
	}
}
