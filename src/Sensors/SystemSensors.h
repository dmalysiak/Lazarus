/*
 * SystemSensors.h
 *
 *  Created on: 07.09.2014
 *      Author: Darius Malysiak
 */

#ifndef SYSTEMSENSORS_H_
#define SYSTEMSENSORS_H_

#include "../BaseObject.h"
#include "../DataStructures/FastKTuple.h"

#include <string>
#include "LibSensors.h"

#ifdef USELMSENSORS
#include <sensors/sensors.h>
#endif

namespace Lazarus {

class SystemSensors: public Lazarus::BaseObject {
public:

	static Lazarus::FastKTuple<std::string> detectSensors();

	SystemSensors(std::string sensor_name = std::string("it8728-isa-0290"));
	virtual ~SystemSensors();

	void updateValues();
	unsigned int getSensorCount();

	double getSensorValue(unsigned int index);
	const double* getAllSensorValues();

	const std::string& getSensorUnit(unsigned int index);
	const Lazarus::FastKTuple<std::string>* getAllSensorUnits();

	const std::string& getSensorName(unsigned int index);
	const Lazarus::FastKTuple<std::string>* getAllSensorNames();

	/**
	 * lists all sensors and their values
	 */
	void fetchVals();

	/**
	 * Returns the unit for the subfeature value
	 */
#ifdef USELMSENSORS
	static std::string getUnitString(const sensors_subfeature* subf);
#endif

#ifdef NOLMSENSORS
	static std::string getUnitString(const void* subf);
#endif

protected:
	LibSensors* mp_lsensors;

	static std::string translateBusType(int type)
	{
		switch(type)
		{
		case -1: return std::string("any");
		case 0: return std::string("i2c");
		case 1: return std::string("isa");
		case 2: return std::string("pci");
		case 3: return std::string("spi");
		case 4: return std::string("virtual");
		case 5: return std::string("acpi");
		case 6: return std::string("hid");
		}

		return "";
	}


#ifdef USELMSENSORS
	const sensors_chip_name* mp_cn;
	const sensors_feature* mp_feat;
	const sensors_subfeature* mp_subf;
#endif

	std::string m_sensor_name;
	double* mp_vals;
	Lazarus::FastKTuple<std::string>* mp_val_units;
	Lazarus::FastKTuple<std::string>* mp_val_names;
	unsigned long long m_val_count;


};

} /* namespace SHHW */

#endif /* SYSTEMSENSORS_H_ */
