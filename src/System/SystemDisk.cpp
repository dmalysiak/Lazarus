/*
 * SystemDisk.cpp
 *
 *  Created on: 27.06.2013
 *      Author: darius
 */

#include "SystemDisk.h"
#include "../Toolbox.h"

#include <string.h>
#include <stdio.h>

#ifdef USEBLOCKID
#include <blkid/blkid.h>
#endif

#include <mntent.h>
#include <stdio.h>
#include <sys/statfs.h>

namespace Lazarus {

DiskPartition::DiskPartition()
{
	m_partition_number = 0;
	m_partition_start = 0;
	m_partition_size = 0;
	m_partition_type = 0;
	m_part_flags = 0;

	m_is_extended = false;
	m_is_primary = false;
	m_is_logical = false;

	m_bavail = 0;
	m_bfree = 0;
	m_blocks = 0;
	m_bsize = 0;
	m_ffree = 0;
	m_files = 0;
	m_fs_flags = 0;
	m_frsize = 0;
	m_type = 0;
	m_namelen = 0;
}

DiskPartition::~DiskPartition()
{

}

void DiskPartition::printInfo()
{
	printf("Dev %s, partition nr %u, partition start %llu, partition size %llu, partition type 0x%x (%s), partition flags %llu "
			"UUID %s, partition name %s, type name %s, mount point %s\n",
			m_device_name.c_str(),
			m_partition_number,
			m_partition_start,
			m_partition_size,
			m_partition_type,getPartitionTypeString(m_partition_type).c_str(),
			m_part_flags,
			m_uuid.c_str(),
			m_partition_name.c_str(),
			m_type_name.c_str(),
			m_mount_point.c_str());

	if(m_is_extended)
		printf("Partition is an extended partition\n");

	if(m_is_primary)
		printf("Partition is a primary partition\n");

	if(m_is_logical)
		printf("Partition is a logical partition\n");

	printf("Filessystem information:\n");

	printf("\t free blocks available to unprivileged user %llu \n",m_bavail);
	printf("\t free blocks in fs %llu \n",m_bfree);
	printf("\t total data blocks in file system %llu \n",m_blocks);
	printf("\t optimal transfer block size %llu \n",m_bsize);
	printf("\t free file nodes in fs %llu \n",m_ffree);
	printf("\t fs flags %llu \n",m_fs_flags);
	printf("\t total file nodes in file system %llu \n",m_files);
	printf("\t fragment size %llu \n",m_frsize);
	printf("\t type of file system %llu (%s) \n",m_type,m_fs_type.c_str());
	printf("\t maximum length of filenames %llu \n",m_namelen);

}

void DiskPartition::serialize()
{
	registerElement<unsigned long long int>(13);
	registerElement<unsigned int>(1);
	registerElement<int>(1);
	registerElement<bool>(3);
	registerString(m_uuid);
	registerString(m_fs_type);
	registerString(m_partition_name);
	registerString(m_type_name);
	registerString(m_mount_point);
	registerString(m_device_name);
	registerUCharA(sizeof(__fsid_t));

	allocateBuffer();

	addUInt(m_partition_number);
	addULongLong(m_partition_start);
	addULongLong(m_partition_size);
	addInt(m_partition_type);
	addString(m_uuid);
	addString(m_partition_name);
	addString(m_type_name);
	addString(m_mount_point);
	addString(m_device_name);
	addULongLong(m_part_flags);

	//filesystem information
	addULongLong(m_bavail);
	addULongLong(m_bfree);
	addULongLong(m_blocks);
	addULongLong(m_bsize);
	addULongLong(m_ffree);
	addULongLong(m_files);
	addULongLong(m_fs_flags);
	addULongLong(m_frsize);
	addUCharA((unsigned char*)&m_fsid,sizeof(__fsid_t));
	addULongLong(m_type);
	addULongLong(m_namelen);
	addString(m_fs_type);

	addBool(m_is_extended);
	addBool(m_is_primary);
	addBool(m_is_logical);
}

void DiskPartition::deserialize()
{

	m_is_logical = getBool();
	m_is_primary = getBool();
	m_is_extended = getBool();
	m_fs_type = getString();
	m_namelen = getULongLong();
	m_type = getULongLong();

	unsigned long long size;
	unsigned char* id = getUCharA(size);
	memcpy(&m_fsid,id,sizeof(__fsid_t));
	delete[] id;

	m_frsize = getULongLong();
	m_fs_flags = getULongLong();
	m_files = getULongLong();
	m_ffree = getULongLong();
	m_bsize = getULongLong();
	m_blocks = getULongLong();
	m_bfree = getULongLong();
	m_bavail = getULongLong();
	m_part_flags = getULongLong();
	m_device_name = getString();
	m_mount_point = getString();
	m_type_name = getString();
	m_partition_name = getString();
	m_uuid = getString();
	m_partition_type = getInt();
	m_partition_size = getULongLong();
	m_partition_start = getULongLong();
	m_partition_number = getUInt();

	resetBuffer();
}


std::string DiskPartition::getPartitionTypeString(unsigned int type)
{
	//in case of an empty table fill it up
	if(DiskPartition::mp_ptype_lookup_table->size() == 0)
	{
		for(int i=0;i<98;++i)
		{
			(*DiskPartition::mp_ptype_lookup_table)[msdos_systypes[i].type] = msdos_systypes[i].name;
		}
	}

	std::string s = "Unknown";

	try{
		s = (*DiskPartition::mp_ptype_lookup_table)[type];
	}
	catch(std::exception& e)
	{
		printf("Unknown partition type\n");
	}

	return s;
}

std::string DiskPartition::getFSTypeString(unsigned int type)
{
	//in case of an empty table fill it up
	if(DiskPartition::mp_fstype_lookup_table->size() == 0)
	{
		for(int i=0;i<45;++i)
		{
			(*DiskPartition::mp_fstype_lookup_table)[fs_types[i].type] = fs_types[i].name;
		}
	}

	std::string s = "Unknown";

	try{
		s = (*DiskPartition::mp_fstype_lookup_table)[type];
	}
	catch(std::exception& e)
	{
		printf("Unknown file system\n");
	}

	return s;
}


SystemDisk::SystemDisk(const std::string& dev_file) {

	m_dev_name = dev_file;

	m_disk_size = 0;
	m_logical_sector_size = 0;
	m_physical_sector_size = 0;
	m_minimum_io_size = 0;
	m_optimal_io_size = 0;
	m_alignment_offset = 0;
	m_table_offset = 0;
	m_partition_type = "";
	m_table_id = "";

	loadDiskInformation();
}

SystemDisk::SystemDisk()
{
	m_dev_name = "";
	m_disk_size = 0;
	m_logical_sector_size = 0;
	m_physical_sector_size = 0;
	m_minimum_io_size = 0;
	m_optimal_io_size = 0;
	m_alignment_offset = 0;
	m_table_offset = 0;
	m_partition_type = "";
	m_table_id = "";
}

SystemDisk::~SystemDisk()
{
	m_partitions.destroy();
}


void SystemDisk::printInfo()
{
	printf("Printing information for device %s\n",m_dev_name.c_str());
	printf("Disk size %llu, partition type %s, PTable ID %s, logical sector size %llu, physical sector size %llu, "
			"minimum io size %llu, optimal io size %llu, alignment offset %llu, PTable offset %llu\n",
			m_disk_size,
			m_partition_type.c_str(),
			m_table_id.c_str(),
			m_logical_sector_size,
			m_physical_sector_size,
			m_minimum_io_size,
			m_optimal_io_size,
			m_alignment_offset,
			m_table_offset);
	printf("Disk has %lu partitions\n",m_partitions.getSize());
	printf("Printing information about partitions\n");
	printf("~~~~~~~~~~~~~~\n");

	for(auto it = m_partitions.getStart();it!=m_partitions.getEnd();++it)
	{
		it.getElement()->getm_data()->printInfo();
		printf("~~~~~~~~~~~~~~\n");
	}
}

void SystemDisk::updateMountLookupTable()
{
    //determine the mount point (if it exists...)
    struct mntent* mount;
	FILE* mount_list = setmntent("/proc/mounts", "r");
	if(mount_list == NULL)
	{
		printf("ERROR: could not get list of mounted devs\n");
		return;
	}
	else
	{
		SystemDisk::mp_mount_lookup_table->clear();

		while((mount = getmntent(mount_list)) != NULL)
		{
			(*SystemDisk::mp_mount_lookup_table)[mount->mnt_fsname] = mount->mnt_dir;
			//printf("%s %s\n", mount->mnt_fsname, mount->mnt_dir);
		}
	}
	endmntent(mount_list);
}

int SystemDisk::loadDiskInformation(bool updateMountTable)
{
#ifdef USEBLOCKID
	//first update the lookup table for the mounted devs
	if(updateMountTable == true)
	{
		updateMountLookupTable();
	}

	blkid_probe pr = blkid_new_probe_from_filename(m_dev_name.c_str());

	if(pr == NULL)
	{
		printf("ERROR: could not create device probe\n");
		return -1;
	}

	//enable topology probing
	int res = blkid_probe_enable_topology(pr,1);
	if(res == -1)
	{
		printf("ERROR: could not enable topology probing\n");
		return -1;
	}

	//get topology
	blkid_topology topology = blkid_probe_get_topology(pr);

	m_alignment_offset = blkid_topology_get_alignment_offset(topology);
	m_logical_sector_size = blkid_topology_get_logical_sector_size(topology);
	m_minimum_io_size = blkid_topology_get_minimum_io_size(topology);
	m_optimal_io_size = blkid_topology_get_optimal_io_size(topology);
	m_physical_sector_size = blkid_topology_get_physical_sector_size(topology);
	m_disk_size = blkid_probe_get_size(pr);

	//get all partitions
	blkid_partlist ls = blkid_probe_get_partitions(pr);
	int partition_count = blkid_partlist_numof_partitions(ls);
	DiskPartition* partition = NULL;

	//get the root partition table
	blkid_parttable root_partition_table = blkid_partlist_get_table(ls);

	m_partition_type = blkid_parttable_get_type(root_partition_table);
	m_table_offset = blkid_parttable_get_offset(root_partition_table);
	m_table_id = blkid_parttable_get_id(root_partition_table);

	//delete any existing entries
	m_partitions.destroy();

	struct statfs64 fs_info;

	for(int i = 0;i < partition_count; ++i)
	{
	     blkid_partition par = blkid_partlist_get_partition(ls, i);
	     partition = new DiskPartition();

	     partition->m_partition_number = blkid_partition_get_partno(par);
	     partition->m_partition_start = blkid_partition_get_start(par);
	     partition->m_partition_size = blkid_partition_get_size(par);
	     partition->m_partition_type = blkid_partition_get_type(par);
	     partition->m_part_flags = blkid_partition_get_flags(par);

	     //if supported
	     const char* s = blkid_partition_get_uuid(par);
	     if(s != NULL)
	     {
	    	 partition->m_uuid = std::string(s);
	     }

	     //if supported
	     s = blkid_partition_get_name(par);
		 if(s != NULL)
		 {
			 partition->m_partition_name = std::string(s);
		 }

		 //if supported
		 s = blkid_partition_get_type_string(par);
		 if(s != NULL)
		 {
			 partition->m_type_name = std::string(s);
		 }

	     if(blkid_partition_is_extended(par)==true)
	     {
	    	 partition->m_is_extended = true;
	     }

	     if(blkid_partition_is_logical(par)==true)
		 {
			 partition->m_is_logical = true;
		 }

	     if(blkid_partition_is_primary(par)==true)
		 {
			 partition->m_is_primary = true;
		 }

	     partition->m_device_name = m_dev_name+Toolbox::intToString(i+1);

	     //get the mount point if the device is mounted
	     partition->m_mount_point = (*SystemDisk::mp_mount_lookup_table)[partition->m_device_name];
	     if(partition->m_mount_point.length() > 0)//if a mount point exists determine the filesystem
	     {
	    	 if(statfs64(partition->m_mount_point.c_str(), &fs_info) != 0)
	    	 {
	    	     printf("ERROR: could not fetch filesystem information\n");
	    	 }
	    	 else
	    	 {
	    		 partition->m_bavail = fs_info.f_bavail;
	    		 partition->m_bfree = fs_info.f_bfree;
	    		 partition->m_blocks = fs_info.f_blocks;
	    		 partition->m_bsize = fs_info.f_bsize;
	    		 partition->m_ffree = fs_info.f_ffree;
	    		 partition->m_files = fs_info.f_files;
	    		 partition->m_fs_flags = fs_info.f_flags;
	    		 partition->m_frsize = fs_info.f_frsize;
	    		 partition->m_fsid = fs_info.f_fsid;
	    		 partition->m_type = fs_info.f_type;
	    		 partition->m_namelen =  fs_info.f_namelen;
	    		 partition->m_fs_type = DiskPartition::getFSTypeString(partition->m_type);
	    	 }
	     }

	     m_partitions.appendLast(partition);
	}

	blkid_free_probe(pr);
#endif
#ifdef NOBLOCKID
      
	m_alignment_offset = 0;
	m_logical_sector_size = 0;
	m_minimum_io_size = 0;
	m_optimal_io_size = 0;
	m_physical_sector_size = 0;
	m_disk_size = 0;
	m_partition_type = "";
	m_table_offset = 0;
	m_table_id = "";

	m_partitions.destroy();
#endif
	return 0;
}

void SystemDisk::serialize()
{
	Serializable::resetBuffer();
	registerElement<unsigned int>(1);
	registerElement<unsigned long long>(7);
	registerString(m_dev_name);
	registerString(m_partition_type);
	registerString(m_table_id);

	for(auto it = m_partitions.getStart(); it!=m_partitions.getEnd(); ++it)
	{
		it.getElement()->getm_data()->serialize();
		registerUCharA(it.getElement()->getm_data()->get_mp_serialized_data_buffer()->get_m_length());
	}

	allocateBuffer();

	addString(m_dev_name);
	addString(m_partition_type);
	addString(m_table_id);
	addULongLong(m_disk_size);
	addULongLong(m_logical_sector_size);
	addULongLong(m_physical_sector_size);
	addULongLong(m_minimum_io_size);
	addULongLong(m_optimal_io_size);
	addULongLong(m_alignment_offset);
	addULongLong(m_table_offset);

	unsigned int partCount = 0;
	for(auto it = m_partitions.getStart(); it!=m_partitions.getEnd(); ++it)
	{
		addUCharA(it.getElement()->getm_data()->get_mp_serialized_data_buffer()->get_mp_data(),
				it.getElement()->getm_data()->get_mp_serialized_data_buffer()->get_m_length() );

		++partCount;
	}

	addUInt(partCount);
}

void SystemDisk::deserialize()
{
	m_partitions.destroy();

	unsigned int partCount = getUInt();

	for(unsigned int i=0;i<partCount;++i)
	{
		DiskPartition* part = new DiskPartition();

		unsigned long long size = 0;
		unsigned char* data = getUCharA(size);
		part->set_mp_serialized_data_buffer(new Buffer(data,size));
		part->deserialize();

		m_partitions.appendLast(part);
	}

	m_table_offset = getULongLong();
	m_alignment_offset = getULongLong();
	m_optimal_io_size = getULongLong();
	m_minimum_io_size = getULongLong();
	m_physical_sector_size = getULongLong();
	m_logical_sector_size = getULongLong();
	m_disk_size = getULongLong();
	m_table_id = getString();
	m_partition_type = getString();
	m_dev_name = getString();

	resetBuffer();
}

DiskPartition::fs_types_ DiskPartition::fs_types[45] = {
		{ "ADFS", 0xadf5 },
		{ "AFFS", 0xADFF },
		{ "BEFS", 0x42465331 },
		{ "BFS", 0x1BADFACE },
		{ "CIFS", 0xFF534D42 },
		{ "CODA", 0x73757245 },
		{ "COH", 0x012FF7B7 },
		{ "CRAMFS", 0x28cd3d45 },
		{ "DEVFS", 0x1373 },
		{ "EFS", 0x00414A53 },
		{ "EXT", 0x137D },
		{ "EXT2_OLD", 0xEF51 },
		{ "EXT2", 0xEF53 },
		{ "EXT3", 0xEF53 },
		{ "EXT4", 0xEF53 },
		{ "HFS", 0x4244 },
		{ "HPFS", 0xF995E849 },
		{ "HUGETLBFS", 0x958458f6 },
		{ "ISOFS", 0x9660 },
		{ "JFFS2", 0x72b6 },
		{ "JFS", 0x3153464a },
		{ "MINIX", 0x137F /* orig. minix */ },
		{ "MINIX2", 0x138F /* 30 char minix */ },
		{ "MINIX2", 0x2468 /* minix V2 */ },
		{ "MINIX22", 0x2478 /* minix V2, 30 char names */ },
		{ "MSDOS", 0x4d44 },
		{ "NCP", 0x564c },
		{ "NFS", 0x6969 },
		{ "NTFS_SB", 0x5346544e },
		{ "OPENPROM", 0x9fa1 },
		{ "PROC", 0x9fa0 },
		{ "QNX4", 0x002f },
		{ "REISERFS", 0x52654973 },
		{ "ROMFS", 0x7275 },
		{ "SMB", 0x517B },
		{ "SYSV2", 0x012FF7B6 },
		{ "SYSV4", 0x012FF7B5 },
		{ "TMPFS", 0x01021994 },
		{ "UDF", 0x15013346 },
		{ "UFS", 0x00011954 },
		{ "USBDEVICE", 0x9fa2 },
		{ "VXFS", 0xa501FCF5 },
		{ "XENIX", 0x012FF7B4 },
		{ "XFS", 0x58465342 },
		{ "_XIAFS", 0x012FD16D }
};


/*Thanks to the guys over at fdisk for assembling this list!!*/
DiskPartition::partition_types DiskPartition::msdos_systypes[98] = {
		{ 0x00, "Empty" },
		{ 0x01, "FAT12" },
		{ 0x02, "XENIX root" },
		{ 0x03, "XENIX usr" },
		{ 0x04, "Small FAT16" },
		{ 0x05, "Extended" },
		{ 0x06, "FAT16" },
		{ 0x07, "HPFS/NTFS" },
		{ 0x08, "AIX" },
		{ 0x09, "AIX bootable" },
		{ 0x0a, "OS/2 boot mgr" },
		{ 0x0b, "FAT32" },
		{ 0x0c, "FAT32 LBA" },
		{ 0x0e, "FAT16 LBA" },
		{ 0x0f, "Extended LBA" },
		{ 0x10, "OPUS" },
		{ 0x11, "Hidden FAT12" },
		{ 0x12, "Compaq diag" },
		{ 0x14, "Hidd Sm FAT16" },
		{ 0x16, "Hidd FAT16" },
		{ 0x17, "Hidd HPFS/NTFS" },
		{ 0x18, "AST SmartSleep" },
		{ 0x1b, "Hidd FAT32" },
		{ 0x1c, "Hidd FAT32 LBA" },
		{ 0x1e, "Hidd FAT16 LBA" },
		{ 0x24, "NEC DOS" },
		{ 0x27, "Hidden NTFS WinRE" },
		{ 0x39, "Plan 9" },
		{ 0x3c, "PMagic recovery" },
		{ 0x40, "Venix 80286" },
		{ 0x41, "PPC PReP Boot" },
		{ 0x42, "SFS" },
		{ 0x4d, "QNX4.x" },
		{ 0x4e, "QNX4.x 2nd part" },
		{ 0x4f, "QNX4.x 3rd part" },
		{ 0x50, "OnTrack DM" },
		{ 0x51, "OnTrackDM6 Aux1" },
		{ 0x52, "CP/M" },
		{ 0x53, "OnTrackDM6 Aux3" },
		{ 0x54, "OnTrack DM6" },
		{ 0x55, "EZ Drive" },
		{ 0x56, "Golden Bow" },
		{ 0x5c, "Priam Edisk" },
		{ 0x61, "SpeedStor" },
		{ 0x63, "GNU HURD/SysV" },
		{ 0x64, "Netware 286" },
		{ 0x65, "Netware 386" },
		{ 0x70, "DiskSec MltBoot" },
		{ 0x75, "PC/IX" },
		{ 0x80, "Minix <1.4a" },
		{ 0x81, "Minix >1.4b" },
		{ 0x82, "Linux swap" },
		{ 0x83, "Linux" },
		{ 0x84, "OS/2 hidden C:" },
		{ 0x85, "Linux extended" },
		{ 0x86, "NTFS volume set" },
		{ 0x87, "NTFS volume set" },
		{ 0x88, "Linux plaintext" },
		{ 0x8e, "Linux LVM" },
		{ 0x93, "Amoeba" },
		{ 0x94, "Amoeba BBT" },
		{ 0x9f, "BSD/OS" },
		{ 0xa0, "Thinkpad hib" },
		{ 0xa5, "FreeBSD" },
		{ 0xa6, "OpenBSD" },
		{ 0xa7, "NeXTSTEP" },
		{ 0xa8, "Darwin UFS" },
		{ 0xa9, "NetBSD" },
		{ 0xab, "Darwin boot" },
		{ 0xaf, "HFS / HFS+" },
		{ 0xb7, "BSDI fs" },
		{ 0xb8, "BSDI swap" },
		{ 0xbb, "Boot Wizard Hid" },
		{ 0xbe, "Solaris boot" },
		{ 0xbf, "Solaris" },
		{ 0xc1, "DRDOS/2 FAT12" },
		{ 0xc4, "DRDOS/2 smFAT16" },
		{ 0xc6, "DRDOS/2 FAT16" },
		{ 0xc7, "Syrinx" },
		{ 0xda, "Non-FS data" },
		{ 0xdb, "CP/M / CTOS" },
		{ 0xde, "Dell Utility" },
		{ 0xdf, "BootIt" },
		{ 0xe1, "DOS access" },
		{ 0xe3, "DOS R/O" },
		{ 0xe4, "SpeedStor" },
		{ 0xfb, "VMware VMFS" },
		{ 0xfc, "VMware VMKCORE" },
		{ 0xeb, "BeOS fs" },
		{ 0xee, "GPT" },
		{ 0xef, "EFI FAT" },
		{ 0xf0, "Lnx/PA-RISC bt" },
		{ 0xf1, "SpeedStor" },
		{ 0xf2, "DOS secondary" },
		{ 0xf4, "SpeedStor" },
		{ 0xfd, "Lnx RAID auto" },
		{ 0xfe, "LANstep" },
		{ 0xff, "XENIX BBT" }
	};

std::unordered_map<unsigned int, std::string>* DiskPartition::mp_ptype_lookup_table = new std::unordered_map<unsigned int, std::string>();
std::unordered_map<unsigned int, std::string>* DiskPartition::mp_fstype_lookup_table = new std::unordered_map<unsigned int, std::string>();
std::unordered_map<std::string, std::string>* SystemDisk::mp_mount_lookup_table = new std::unordered_map<std::string, std::string>();

} /* namespace Lazarus */


