/*
 * SystemDisk.h
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#ifndef SYSTEMDISK_H_
#define SYSTEMDISK_H_

#include "../Serializable.h"
#include "../DataStructures/FastNCSList.h"

#include <string>
#include <unordered_map>

namespace Lazarus {

/**
 * One may require root privileges to fully parse the disk information.
 * */

class DiskPartition: public Serializable
{
public:

	struct partition_types {
		unsigned char type;
		const char *name;
	};

	struct fs_types_ {
		const char *name;
		unsigned long long int type;
	};

	//it's a singleton...: the list will be filled upon the first call to getPartitionTypeString
	static std::unordered_map<unsigned int, std::string>* mp_ptype_lookup_table;

	//it's a singleton...: the list will be filled upon the first call to getFSTypeString
	static std::unordered_map<unsigned int, std::string>* mp_fstype_lookup_table;

	static partition_types msdos_systypes[98];

	static fs_types_ fs_types[45];

	DiskPartition();
	~DiskPartition();

	/**
	 * Prints information about the partition.
	 * */
	void printInfo();

	/**
	 * Returns the string to the partition type according to the standard type list.
	 * */
	static std::string getPartitionTypeString(unsigned int type);

	/**
	 * Returns the string to the filesystem type according to the standard type list.
	 * */
	static std::string getFSTypeString(unsigned int type);

	//partition information
	unsigned int m_partition_number;
	unsigned long long int m_partition_start;
	unsigned long long int m_partition_size;
	int m_partition_type;
	std::string m_uuid;//if supported (GPT)!
	std::string m_partition_name;//if supported (GPT)!
	std::string m_type_name;//if supported (GPT)!
	std::string m_mount_point;
	std::string m_device_name;
	unsigned long long int m_part_flags;

	//filesystem information
	unsigned long long int m_bavail; /* free blocks available to unprivileged user */
	unsigned long long int m_bfree; /* free blocks in fs */
	unsigned long long int m_blocks; /* total data blocks in file system */
	unsigned long long int m_bsize;/* optimal transfer block size */
	unsigned long long int m_ffree; /* free file nodes in fs */
	unsigned long long int m_files;/* total file nodes in file system */
	unsigned long long int m_fs_flags;
	unsigned long long int m_frsize; /* fragment size (since Linux 2.6) */
	__fsid_t m_fsid; /* file system id */
	unsigned long long int m_type; /* type of file system (see below) */
	unsigned long long int m_namelen; /* maximum length of filenames */
	std::string m_fs_type;/*string representation for filesystem type*/

	bool m_is_extended;
	bool m_is_primary;
	bool m_is_logical;



	virtual void serialize();
	virtual void deserialize();
};

class SystemDisk: public Serializable {
public:
	//key is the partition device string; it's a singleton
	static std::unordered_map<std::string, std::string>* mp_mount_lookup_table;

	/**
	 * This constructor will fetch all accessible data from the system for
	 * the associated device 'dev_file'.
	 * */
	SystemDisk(const std::string& dev_file);

	/**
	 * This constructor will not fetch any data from the system as the object is not
	 * associated with any device yet!
	 * Use this constructor for e.g. creation of empty husks which are going to be
	 * filled via 'deserialize'.
	 * */
	SystemDisk();

	virtual ~SystemDisk();

	/**
	 * This will update the static list of all mounted devices. If one desires a list of all
	 * mounted devs, simply call this method and access the member mp_mount_lookup_table!
	 * */
	void updateMountLookupTable();

	/**
	 * Updates the information for the currently set device 'm_dev_name' by probing it.
	 * updateMountTable determines if the mount lookup table should be updated, this is a costly operation...
	 * Returns 0 in case of success, -1 otherwise.
	 * */
	int loadDiskInformation(bool updateMountTable=true);

	/**
	 * Prints information about the current disk.
	 * */
	void printInfo();

	virtual void serialize();
	virtual void deserialize();

	FastNCSList<DiskPartition*> m_partitions;
	std::string m_dev_name;
	std::string m_partition_type;//dos gpt ...
	std::string m_table_id;
	unsigned long long m_disk_size;
	//this is the smallest unit the storage device can address. It is typically 512 bytes.
	unsigned long long m_logical_sector_size;
	//this is the smallest unit a physical storage device can write atomically. It is usually the same as the logical sector size but may be bigger.
	unsigned long long m_physical_sector_size;
	//minimum size which is the device's preferred unit of I/O. For RAID arrays it is often the stripe chunk size.
	unsigned long long m_minimum_io_size;
	//usually the stripe width for RAID or zero. For RAID arrays it is usually the stripe width or the internal track size.
	unsigned long long m_optimal_io_size;
	//indicates how many bytes the beginning of the device is offset from the disk's natural alignment.
	unsigned long long m_alignment_offset;
	//offset of the partition table
	unsigned long long m_table_offset;

};

} /* namespace Lazarus */
#endif /* SYSTEMDISK_H_ */
