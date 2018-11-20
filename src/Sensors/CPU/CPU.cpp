/*
 * CPU.cpp
 *
 *  Created on: 06.09.2014
 *      Author: Darius Malysiak
 */

#include "CPU.h"

#include <iostream>

namespace Lazarus {

CPU::CPU(std::string cpu_name, std::string CPU_sensor) : SystemSensors(CPU_sensor)
{
	m_CPU_name = cpu_name;
}

CPU::~CPU()
{

}


void CPU::updateTemperatures()
{
	updateValues();
}

unsigned int CPU::getSensorCount()
{
	return m_val_count;
}

double CPU::getSensorTemp(unsigned int index)
{
	return getSensorValue(index);
}

const double* CPU::getAllTemps(unsigned int& count)
{
	count = m_val_count;
	return mp_vals;
}

} /* namespace SHHW */
