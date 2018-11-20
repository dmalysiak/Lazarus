/*
 * RCSServerCallback.h
 *
 *  Created on: Aug 30, 2013
 *      Author: Darius Malysiak
 */

#ifndef RCSSERVERCALLBACK_H_
#define RCSSERVERCALLBACK_H_

#include "../GenericCallback.h"
#include "../../DataStructures/NCSList.h"
#include "../../System/TinyNode.h"
#include "../UDPSocket.h"

#include <string>

namespace Lazarus {

struct RCSCallbackData
{
	unsigned int m_actual_buffer_content_length;
	Lazarus::Buffer* mp_data_buffer;
	UDPSocket* mp_socket;
};

class RCSServerCallback: public Lazarus::GenericCallback {
public:
	RCSServerCallback();
	virtual ~RCSServerCallback();

	virtual int call(Lazarus::Thread* t,	void* var);

	Lazarus::NCSList<Lazarus::TinyNode*>* getNodes();

private:
	bool checkIfNodeExists(std::string ip);

	Lazarus::NCSList<Lazarus::TinyNode*>* mp_nodes;
};

} /* namespace Lazarus */
#endif /* RCSSERVERCALLBACK_H_ */
