/*
 * SimpleXMLExtended.h
 *
 *  Created on: Oct 10, 2013
 *      Author: clustro
 */

#ifndef SIMPLEXMLEXTENDED_H_
#define SIMPLEXMLEXTENDED_H_

#include "SimpleXML.h"

namespace Lazarus {

class SimpleXMLExtended: public Lazarus::SimpleXML {
public:
	SimpleXMLExtended();
	virtual ~SimpleXMLExtended();

	//vectorized version for searching a node by a given position
    int readNodeContent(std::vector<std::vector<std::string>> &path_segments, std::vector<unsigned int> sibling_nrs,
    		std::vector<std::string> &segment_end_node_names, std::string &value);

    //vectorized version for searching a node by a childnodes content
    int readNodeContentByChildnodesContent(std::vector<std::vector<std::string>> &path_segments, std::vector<std::string> childnode_name, std::vector<std::string> childnode_content,
        		std::vector<std::string> &segment_end_node_names, std::string &value);

};

} /* namespace Lazarus */
#endif /* SIMPLEXMLEXTENDED_H_ */
