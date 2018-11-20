/*
 * SystemSensors.cpp
 *
 *  Created on: 07.09.2014
 *      Author: Darius Malysiak
 */

#include "SystemSensors.h"
#include "../Toolbox.h"

#include <iostream>

namespace Lazarus {

#ifdef USELMSENSORS

Lazarus::FastKTuple<std::string> SystemSensors::detectSensors()
{
	//LibSensors* lsensors = LibSensors::getInstance();

	Lazarus::FastKTuple<std::string> names;

	sensors_chip_name const * cn;
	int c = 0;
	while( (cn = sensors_get_detected_chips(0, &c)) != 0)
	{
		names.push(std::string(cn->prefix) + "-" + translateBusType(cn->bus.type) + "-" + Lazarus::Toolbox::numToString<short>(cn->addr,4));
	}

	return names;
}

SystemSensors::SystemSensors(std::string sensor_name) {

	mp_lsensors = LibSensors::getInstance();

	mp_cn = NULL;
	mp_feat = NULL;
	mp_subf = NULL;
	m_val_count = 0;

	m_sensor_name = sensor_name;

	//get the chip name
	sensors_chip_name cn;
	int res = sensors_parse_chip_name(m_sensor_name.c_str(), &cn);//

	if(res != 0)
	{
		printf("ERROR:could not get chip name\n");
	}

	int stupid_val = 0;
	mp_cn = sensors_get_detected_chips(&cn, &stupid_val);

	sensors_free_chip_name(&cn);

	//count the sensors
	int f = 0;

	while ((mp_feat = sensors_get_features(mp_cn, &f)) != NULL)
	{
		int s = 0;

		while((mp_subf = sensors_get_all_subfeatures(mp_cn, mp_feat, &s)) != NULL)
		{
			if((mp_subf->flags & SENSORS_MODE_R) == SENSORS_MODE_R)
			{
				++m_val_count;

				//debug
				/*double val;
				res = sensors_get_value(mp_cn, mp_subf->number, &val);
				printf("1sensor value %f\n",val);*/
			}
		}
	}


	mp_vals = new double[m_val_count];
	mp_val_units = new Lazarus::FastKTuple<std::string>(m_val_count);
	mp_val_names = new Lazarus::FastKTuple<std::string>(m_val_count);

	//one more round for the strings and initial values
	f = 0;
	double val = 0;
	mp_feat = NULL;
	mp_subf = NULL;

	//read the sensor values
	unsigned int i=0;
	while ((mp_feat = sensors_get_features(mp_cn, &f)) != NULL)
	{
		int s = 0;

		while((mp_subf = sensors_get_all_subfeatures(mp_cn, mp_feat, &s)) != NULL)
		{
			if((mp_subf->flags & SENSORS_MODE_R) == SENSORS_MODE_R)
			{
				res = sensors_get_value(mp_cn, mp_subf->number, &val);

				if (res < 0)
				{
					printf("ERROR: could not read sensor value: %d\n",res);
				}
				else
				{
					mp_vals[i]=val;
					(*mp_val_units)[i] = getUnitString(mp_subf);
					(*mp_val_names)[i] = std::string(mp_subf->name);
					//printf("sensor name %s\n",(*mp_val_units)[i].c_str());
				}
				++i;
			}
		}
	}

	//debug
	/*for(int i=0;i<m_val_count;++i)
	{
		printf("AsdASD %d : %s : %d\n",i,(*mp_val_units)[i].c_str(),(*mp_val_units)[i].size());
	}*/

}

SystemSensors::~SystemSensors()
{
	DELETE_ARRAY_NULL_CHECKING(mp_vals);
	delete mp_val_units;
	delete mp_val_names;
}

std::string SystemSensors::getUnitString(const sensors_subfeature* subf)
{
	switch(subf->type)
	{

		case SENSORS_SUBFEATURE_IN_INPUT:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_MIN:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_MAX:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_LCRIT:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_CRIT:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_AVERAGE:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_LOWEST:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_HIGHEST:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_ALARM:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_MIN_ALARM:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_MAX_ALARM:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_BEEP:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_LCRIT_ALARM:
			return std::string("V");

		case SENSORS_SUBFEATURE_IN_CRIT_ALARM:
			return std::string("V");


		case SENSORS_SUBFEATURE_FAN_INPUT:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_MIN:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_MAX:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_ALARM:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_FAULT:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_DIV:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_BEEP:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_PULSES:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_MIN_ALARM:
			return std::string("RPM");

		case SENSORS_SUBFEATURE_FAN_MAX_ALARM:
			return std::string("RPM");


		case SENSORS_SUBFEATURE_TEMP_INPUT:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_MAX:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_MAX_HYST:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_MIN:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_CRIT:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_CRIT_HYST:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_LCRIT:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_EMERGENCY:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_EMERGENCY_HYST:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_LOWEST:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_HIGHEST:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_ALARM:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_MAX_ALARM:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_MIN_ALARM:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_CRIT_ALARM:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_FAULT:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_TYPE:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_OFFSET:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_BEEP:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_EMERGENCY_ALARM:
			return std::string("C°");

		case SENSORS_SUBFEATURE_TEMP_LCRIT_ALARM:
			return std::string("C°");


		case SENSORS_SUBFEATURE_POWER_AVERAGE:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_AVERAGE_HIGHEST:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_AVERAGE_LOWEST:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_INPUT:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_INPUT_HIGHEST:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_INPUT_LOWEST:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_CAP:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_CAP_HYST:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_MAX:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_CRIT:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_AVERAGE_INTERVAL:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_ALARM:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_CAP_ALARM:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_MAX_ALARM:
			return std::string("W");

		case SENSORS_SUBFEATURE_POWER_CRIT_ALARM:
			return std::string("W");


		case SENSORS_SUBFEATURE_ENERGY_INPUT:
			return std::string("W");

		case SENSORS_SUBFEATURE_CURR_INPUT:
			return std::string("W");


		case SENSORS_SUBFEATURE_CURR_MIN:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_MAX:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_LCRIT:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_CRIT:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_AVERAGE:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_LOWEST:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_HIGHEST:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_ALARM:
			return std::string("A");


		case SENSORS_SUBFEATURE_CURR_MIN_ALARM:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_MAX_ALARM:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_BEEP:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_LCRIT_ALARM:
			return std::string("A");

		case SENSORS_SUBFEATURE_CURR_CRIT_ALARM:
			return std::string("A");


		case SENSORS_SUBFEATURE_HUMIDITY_INPUT:
			return std::string("");

		case SENSORS_SUBFEATURE_VID:
			return std::string("");


		case SENSORS_SUBFEATURE_INTRUSION_ALARM:
			return std::string("");

		case SENSORS_SUBFEATURE_INTRUSION_BEEP:
			return std::string("");


		case SENSORS_SUBFEATURE_BEEP_ENABLE:
			return std::string("");


		case SENSORS_SUBFEATURE_UNKNOWN :
			return std::string("??");

		default:
			return std::string("ERROR");
	}

	return std::string("ERROR");
}

void SystemSensors::updateValues()
{
	double val = 0;
	mp_feat = NULL;
	mp_subf = NULL;

	//read the sensor values
	int f = 0;
	int s = 0;
	int res = 0;
	unsigned int i=0;

	while((mp_feat = sensors_get_features(mp_cn, &f)) != NULL)
	{
		s = 0;

		while((mp_subf = sensors_get_all_subfeatures(mp_cn, mp_feat, &s)) != NULL)
		{
			if((mp_subf->flags & SENSORS_MODE_R) == SENSORS_MODE_R)
			{
				res = sensors_get_value(mp_cn, mp_subf->number, &val);

				if (res < 0)
				{
					printf("ERROR: could not read sensor value: %d\n",res);
				}
				else
				{
					mp_vals[i] = val;
					//printf("sensor value %f\n",val);
				}
				++i;
			}
		}
	}
}

unsigned int SystemSensors::getSensorCount()
{
	return m_val_count;
}



double SystemSensors::getSensorValue(unsigned int index)
{
	return mp_vals[index];
}


const double* SystemSensors::getAllSensorValues()
{
	return mp_vals;
}



const std::string& SystemSensors::getSensorUnit(unsigned int index)
{
	return (*mp_val_units)[index];
}


const Lazarus::FastKTuple<std::string>* SystemSensors::getAllSensorUnits()
{
	return mp_val_units;
}



const std::string& SystemSensors::getSensorName(unsigned int index)
{
	return (*mp_val_names)[index];
}


const Lazarus::FastKTuple<std::string>* SystemSensors::getAllSensorNames()
{
	return mp_val_names;
}



void SystemSensors::fetchVals()
{
    sensors_chip_name const * cn;
    int c = 0;

    while ((cn = sensors_get_detected_chips(0, &c)) != 0)
    {
        std::cout << "Chip: " << mp_cn->prefix << "/" << mp_cn->path << std::endl;

        sensors_feature const *feat;
        int f = 0;

        while ((feat = sensors_get_features(mp_cn, &f)) != 0) {
            std::cout << f << ": " << feat->name << std::endl;

            sensors_subfeature const *subf;
            int s = 0;

            while ((subf = sensors_get_all_subfeatures(mp_cn, feat, &s)) != 0) {
                std::cout << f << ":" << s << ":" << subf->name
                          << "/" << subf->number << " = ";
                double val;
                if (subf->flags & SENSORS_MODE_R) {
                    int rc = sensors_get_value(mp_cn, subf->number, &val);
                    if (rc < 0) {
                        std::cout << "err: " << rc;
                    } else {
                        std::cout << val;
                    }
                }
                std::cout << std::endl;
            }
        }
    }
}
#endif

#ifdef NOLMSENSORS

SystemSensors::SystemSensors(std::string sensor_name) {

	mp_lsensors = LibSensors::getInstance();

	m_val_count = 0;

	m_sensor_name = sensor_name;

	mp_vals = new double[m_val_count];
	mp_val_units = new Lazarus::FastKTuple<std::string>(m_val_count);
	mp_val_names = new Lazarus::FastKTuple<std::string>(m_val_count);

	//debug
	/*for(int i=0;i<m_val_count;++i)
	{
		printf("AsdASD %d : %s : %d\n",i,(*mp_val_units)[i].c_str(),(*mp_val_units)[i].size());
	}*/

}

SystemSensors::~SystemSensors()
{
	DELETE_ARRAY_NULL_CHECKING(mp_vals);
	delete mp_val_units;
	delete mp_val_names;
}

std::string SystemSensors::getUnitString(const void* subf)
{
	return std::string("NO SENSORS!");
}

void SystemSensors::updateValues()
{

}

unsigned int SystemSensors::getSensorCount()
{
	return m_val_count;
}



double SystemSensors::getSensorValue(unsigned int index)
{
	return mp_vals[index];
}


const double* SystemSensors::getAllSensorValues()
{
	return mp_vals;
}



const std::string& SystemSensors::getSensorUnit(unsigned int index)
{
	return (*mp_val_units)[index];
}


const Lazarus::FastKTuple<std::string>* SystemSensors::getAllSensorUnits()
{
	return mp_val_units;
}



const std::string& SystemSensors::getSensorName(unsigned int index)
{
	return (*mp_val_names)[index];
}


const Lazarus::FastKTuple<std::string>* SystemSensors::getAllSensorNames()
{
	return mp_val_names;
}



void SystemSensors::fetchVals()
{

}
#endif

} /* namespace SHHW */
