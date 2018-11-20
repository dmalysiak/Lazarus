/*
 * CPU.h
 *
 *  Created on: 06.09.2014
 *      Author: Darius Malysiak
 */

#ifndef CPU_H_
#define CPU_H_

#include <string>
#include "../LibSensors.h"
#include "../SystemSensors.h"

#ifdef HW_USE_LMSENSORS
#include <sensors/sensors.h>
#endif

namespace Lazarus {

class CPU: public SystemSensors {
public:
	CPU(std::string cpu_name, std::string CPU_core_sensor = std::string("coretemp-isa-0000"));
	virtual ~CPU();

	void updateTemperatures();
	unsigned int getSensorCount();
	double getSensorTemp(unsigned int index);
	const double* getAllTemps(unsigned int& count);


protected:
	std::string m_CPU_name;
};

} /* namespace SHHW */

#endif /* CPU_H_ */
