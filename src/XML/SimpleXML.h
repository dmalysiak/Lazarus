/*
 * SimpleXMLReader.h
 *
 *  Created on: 21.05.2013
 *      Author: dariusmalysiak
 */

#ifndef SIMPLEXMLREADER_H_
#define SIMPLEXMLREADER_H_

#include "../BaseObject.h"

#include <stdio.h>
#include <pthread.h>
#include <libxml2/libxml/parser.h>
#include <libxml2/libxml/tree.h>
#include <string>
#include <vector>

namespace Lazarus
{

class SimpleXML : public Lazarus::BaseObject {
public:
    SimpleXML();
    virtual ~SimpleXML();

    /**
     * Call this method ONCE in case of multithreaded programs.
     * */
    static void init();

    /**
     * Call this method once the program is about to never use any xml function again.
     * */
    static void free();

    /**
     * Reads the given XML document and parses it into an XML tree. Any existing document structure
     * will be deleted.
     * */
    int parseFile(const std::string &filename);
    int nextNode();
    int childNode();
    void listNodeAttributes(xmlNodePtr node);
    void printXMLTree(xmlNodePtr node);

    /**
     * Creates an empty tree with a root node of the given name. Any existing document structure
     * will be deleted.
     * */
    int createEmptyTree(const std::string& root_node);
    int saveXMLTree(const std::string& filename);

    /**
     * Returns the XML document as a string.
     * */
    std::string retrieveXMLString();

    xmlNodePtr getRootElement();
    xmlNodePtr get_m_current_node();
    void reset_current_node();

    //methods which use a counting argument for finding the right entry in the path
    int readNodeAttribute(std::vector<std::string> &path, const std::string &node_name,const std::string &attribute, std::string &value, unsigned int sibling_nr = 0);
    int readNodeContent(std::vector<std::string> &path, const std::string &node_name, std::string &value, unsigned int sibling_nr = 0);

    int changeNodeAttribute(std::vector<std::string> &path, const std::string &node_name,const std::string &attribute,const std::string &value, unsigned int sibling_nr = 0);
    int addNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, const std::string &value, unsigned int sibling_nr = 0);
    int removeNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, unsigned int sibling_nr = 0);

    int changeNodeContent(std::vector<std::string> &path,const std::string &node_name, const std::string &value, unsigned int sibling_nr = 0);
    int addNodeContent(std::vector<std::string> &path,const std::string &node_name, const std::string &value, unsigned int sibling_nr = 0);
    int removeNodeContent(std::vector<std::string> &path,const std::string &node_name, unsigned int sibling_nr = 0);

    int addNode(std::vector<std::string> &path,const std::string &node_name,const std::string &new_node_name, unsigned int sibling_nr = 0);
    int addNodeSibling(std::vector<std::string> &path,const std::string &node_name,const std::string &new_node_name, unsigned int sibling_nr = 0);
    int removeNode(std::vector<std::string> &path,const std::string &node_name, unsigned int sibling_nr = 0);


protected:
    pthread_mutex_t m_mutex;
    std::string m_filename;
    bool m_is_open;
    unsigned int m_depth;

    xmlDocPtr m_doc;
    xmlNodePtr m_root_element;

    xmlNodePtr m_current_node;
    xmlParserCtxtPtr m_ctxt;

    bool m_stop_search;
    unsigned int m_sibling_counter;
    bool m_child_check_ok;

    void printTabs(unsigned int count);
    void listNodeAttributes__(xmlNodePtr node);
    int getNodeAttribute(xmlNodePtr node,const std::string &attribute, std::string &value);
    int setNodeAttribute(xmlNodePtr node,const std::string &attribute, const std::string &value);
    int removeTextChild(xmlNodePtr node);
    int removeNodeAttribute_(xmlNodePtr node,const std::string &attribute);

    int getTextContent(xmlNodePtr node, std::string &value);
    int setTextContent(xmlNodePtr node, const std::string &value);
    //int readNodeAttributeValue_(std::vector<std::string> &path,xmlNodePtr node, std::string &node_name, std::string &attribute, std::string &value);
    //int readNodeContent_(std::vector<std::string> &path,xmlNodePtr node, std::string &node_name, std::string &value);

    /*search with a position parameter for the desired node, i.e. the method finds the 'sibling_nr'-th node with
    * the specified name in the given path
    */
    int findNode(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, unsigned int sibling_nr = 0);
    int findNode_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, unsigned int sibling_nr = 0);

    int findNodeByChildnodesContent(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string childcontent);
    int findNodeByChildnodesContent_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string childcontent);

    int findNodeByChildnodesAttribute(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string child_attribute);
	int findNodeByChildnodesAttribute_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string child_attribute);

};

}

#endif /* SIMPLEXMLREADER_H_ */
