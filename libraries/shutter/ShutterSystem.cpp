// ShutterSystem.cpp
//
// Copyright (C) 2015 Mike McCauley
// $Id: ShutterSystem.cpp,v 1.2 2015/10/04 05:16:38 mikem Exp mikem $

#include "ShutterSystem.h"
#include "shutter.h"

ShutterSystem::ShutterSystem()
    : _num_shutters(0)
{
}

boolean ShutterSystem::addShutter(shutter& shutter)
{
    if (_num_shutters >= SHUTTERSYSTEM_MAX_SHUTTERS)
	return false; // No room for more
    _shutters[_num_shutters++] = &shutter;
	return true;
}

void ShutterSystem::setMaxSpeed(float speed)
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	_shutters[i]->setMaxSpeed(speed);
	}
}

void ShutterSystem::setSpeed(float speed)
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	_shutters[i]->setSpeed(speed);
	}
}

void ShutterSystem::retrieveSpeed()
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	_shutters[i]->setSpeed(speed_array[i]);
	}
}

void ShutterSystem::saveSpeed()
{
	uint8_t i;
	
    for (i = 0; i < _num_shutters; i++)
    {
	speed_array[i] = _shutters[i]->speed();
	}
}

uint8_t ShutterSystem::checkEndOfRoad()
{
	uint8_t i;
	uint8_t ret = 0;
	
    for (i = 0; i < _num_shutters; i++)
    {
		if ((_shutters[i]->speed() > 0 && _shutters[i]->position() == 1) || (_shutters[i]->speed() < 0 && _shutters[i]->position() == 2))
		{
		_shutters[i]->setSpeed(0);
		ret = 1;
		}
	}
	return ret;
}

void ShutterSystem::runSpeed()
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	_shutters[i]->runSpeed();
	}
}

boolean ShutterSystem::isRunning()
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	if (_shutters[i]->speed() != 0)
		return true;
	}
	return false;
}

boolean ShutterSystem::isError()
{
	uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	if (_shutters[i]->isError())
		return true;
	}
	return false;
}

void ShutterSystem::moveTo(long absolute[])
{
    // First find the shutter that will take the longest time to move
    float longestTime = 0.0;

    uint8_t i;
    for (i = 0; i < _num_shutters; i++)
    {
	long thisDistance = absolute[i] - _shutters[i]->currentPosition();
	float thisTime = abs(thisDistance) / _shutters[i]->maxSpeed();

	if (thisTime > longestTime)
	    longestTime = thisTime;
    }

    if (longestTime > 0.0)
    {
	// Now work out a new max speed for each shutter so they will all 
	// arrived at the same time of longestTime
	for (i = 0; i < _num_shutters; i++)
	{
	    long thisDistance = absolute[i] - _shutters[i]->currentPosition();
	    float thisSpeed = thisDistance / longestTime;
	    _shutters[i]->moveTo(absolute[i]); // New target position (resets speed)
	    _shutters[i]->setSpeed(thisSpeed); // New speed
	}
    }
}

// Returns true if any motor is still running to the target position.
boolean ShutterSystem::run()
{
    uint8_t i;
    boolean ret = false;
    for (i = 0; i < _num_shutters; i++)
    {
	if ( _shutters[i]->distanceToGo() != 0)
	{
	    _shutters[i]->runSpeed();
	    ret = true;
	}
    }
    return ret;
}

// Blocks until all shutters reach their target position and are stopped
void    ShutterSystem::runSpeedToPosition()
{ 
    while (run())
	;
}

void ShutterSystem::systemStatus(){
	uint8_t i;
    
    for (i = 0; i < _num_shutters; i++){
		if (_shutters[i]->speed() != 0)
			Serial.println("Shutter " + String(i + 1) + " is running " + (_shutters[i]->speed() > 0 ? "up" : "down"));
		else
			switch (_shutters[i]->position()){
				case 1:
					Serial.println("Shutter " + String (i + 1) + " is up");
					break;
				case 2:
					Serial.println("Shutter " + String (i + 1) + " is down");
					break;
				default:
					Serial.println("Shutter " + String (i + 1) + " position unknown");
			}
    }
}

void ShutterSystem::systemStatusRawData(){
	uint8_t i;
    
    for (i = 0; i < _num_shutters; i++){
		if (_shutters[i]->speed() != 0)
			Serial.print(_shutters[i]->speed() > 0 ? 1 : 2);
		else
			switch (_shutters[i]->position()){
				case 1:
					Serial.print(3);
					break;
				case 2:
					Serial.print(4);
					break;
				default:
					Serial.print(0);
			}
		if (i != _num_shutters - 1)
			Serial.print(",");
    }
}

bool ShutterSystem::shutterPositionNotEqual(uint8_t pos){
	uint8_t i;
	for (i = 0; i < _num_shutters; i++)
		if (_shutters[i]->position() != pos)
			return true;
	return false;
}
