/*
 * File.h
 *
 *  Created on: Jun 3, 2014
 *      Author: Darius Malysiak
 */

#ifndef FILE_H_
#define FILE_H_

#include "../BaseObject.h"

#include <string>
#include <sys/mman.h>

namespace Lazarus {

/*
 * This class represents a single file and provides access methods to its contents. It is designed in a static and
 * monolithic way, i.e. the filename can not be changed for an object and the access methods follow a tight set of rules.
 */

class File: public Lazarus::BaseObject {
public:
	File(const std::string& file);
	virtual ~File();

	/*
	 * Creates a mapping of a binary file. This should be considered for huge files as only the required file parts will be loaded
	 * (i.e. on demand). Otherwise the whole file would be loaded from disk. Keep in mind that the required
	 * amount of virtual memory remains unchanged.
	 * Furthermore, by default the file will be mapped in a shared fashion, i.e. all changes will be written directly into the
	 * file. For the complementary behavior one should set flags to 'MAP_PRIVATE'.
	 * Any call to this function will unmap any existing mapping and delete any previously loaded data; this does not
	 * necessarily imply a synchronization of the mapped file.
	 * Returns: -1 in case of errors, -2 if a new file was created, 0 in case of success
	 */
	int mapFile(int flags = MAP_SHARED, unsigned long offset = 0, unsigned long size = 0);

	/*
	 * This will force a synchronization of all changes in case of a 'shared' privately mapped file.
	 * Returns: -1 in case of errors, 0 in case of success
	 */
	int syncMap();

	/*
	 * This method will (binary) read the files content into memory. If it has been previously mapped, it
	 * will be unmapped. This does not necessarily imply a synchronization of the mapped file. Furthermore any
	 * previously loaded data will be deleted.
	 * Returns: -1 in case of errors, -2 if a new file was created, 0 in case of success
	 */
	int readFile(unsigned long offset = 0, unsigned long size = 0);

	/*
	 * This method will (binary) write all data back into the file. Will fail if file is mapped.
	 * Returns: -1 in case of errors, 0 in case of success
	 */
	int writeFile();

	/*
	 * This method assigns a new data buffer to an unmapped file. All previously loaded data is discarded and the
	 * object takes ownership of the specified data.
	 */
	void assignData(unsigned char* data, unsigned long size);

	unsigned long getSize();
	unsigned char* getData();

private:
	std::string m_filename;
	unsigned char* mp_data;
	unsigned long m_data_size;
	bool m_is_mapped;
};

} /* namespace Lazarus */

#endif /* FILE_H_ */
