/*
 * SimpleXMLExtended.cpp
 *
 *  Created on: Oct 10, 2013
 *      Author: clustro
 */

#include "SimpleXMLExtended.h"

namespace Lazarus {

SimpleXMLExtended::SimpleXMLExtended() {
	// TODO Auto-generated constructor stub

}

SimpleXMLExtended::~SimpleXMLExtended() {
	// TODO Auto-generated destructor stub
}

int SimpleXMLExtended::readNodeContent(std::vector<std::vector<std::string>> &path_segments, std::vector<unsigned int> sibling_nrs,
    		std::vector<std::string> &segment_end_node_names, std::string &value)
{

	//sanity check
	if( ( path_segments.size() != sibling_nrs.size() ) ||
		( segment_end_node_names.size() != sibling_nrs.size() ) )
	{
		printf("ERROR: vector size mismatch\n");
		return -1;
	}

	int result = 0;
	xmlNodePtr node = NULL;
	xmlNodePtr start_node = m_root_element;

	pthread_mutex_lock(&m_mutex);

	for(unsigned int i=0;i < path_segments.size();i++)
	{
		//debug
		/*printf("trying to find node '%s' with path ",segment_end_node_names.at(i).c_str());
		for(unsigned int j=0;j<path_segments.at(i).size();j++)
		{
			printf("%s -> ",path_segments.at(i).at(j).c_str());
		}
		printf("and number %d\n",sibling_nrs.at(i));*/
		//---------------

		result = findNode( path_segments.at(i), start_node, segment_end_node_names.at(i), node, sibling_nrs.at(i) );

		if(result == -1)
		{
		    //printf("node %s not found\n",segment_end_node_names.at(i).c_str());
		    pthread_mutex_unlock(&m_mutex);

		    return result;
		}

		//update the start node
		start_node = node;
	}

	//grab the content
	result = getTextContent(node, value);

	pthread_mutex_unlock(&m_mutex);

	return result;

}

int SimpleXMLExtended::readNodeContentByChildnodesContent(std::vector<std::vector<std::string>> &path_segments, std::vector<std::string> childnode_name, std::vector<std::string> childnode_content,
        		std::vector<std::string> &segment_end_node_names, std::string &value)
{
	//sanity check
	if( ( path_segments.size() != childnode_name.size() ) ||
		( childnode_name.size() != childnode_content.size() ) ||
		( childnode_content.size() != segment_end_node_names.size() ) )
	{
		printf("ERROR: vector size mismatch\n");
		return -1;
	}

	int result = 0;
	xmlNodePtr node = NULL;
	xmlNodePtr start_node = m_root_element;

	pthread_mutex_lock(&m_mutex);

	for(unsigned int i=0;i < path_segments.size();i++)
	{
		//debug
		/*printf("trying to find node '%s' with path ",segment_end_node_names.at(i).c_str());
		for(unsigned int j=0;j<path_segments.at(i).size();j++)
		{
			printf("%s -> ",path_segments.at(i).at(j).c_str());
		}
		printf("and child node '%s' with content '%s'\n",childnode_name.at(i).c_str(),childnode_content.at(i).c_str());*/
		//---------------

		result = findNodeByChildnodesContent( path_segments.at(i), start_node, segment_end_node_names.at(i), node,
												childnode_name.at(i),childnode_content.at(i) );

		if(result == -1)
		{
			//printf("node %s not found %d\n",segment_end_node_names.at(i).c_str(),i);
			pthread_mutex_unlock(&m_mutex);

			return result;
		}

		//update the start node
		start_node = node;
	}

	//grab the content
	result = getTextContent(node, value);

	pthread_mutex_unlock(&m_mutex);

	return result;
}

} /* namespace Lazarus */




