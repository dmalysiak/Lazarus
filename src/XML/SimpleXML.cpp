/*
 * SimpleXMLReader.cpp
 *
 *  Created on: 21.05.2013
 *      Author: dariusmalysiak
 */

#include "SimpleXML.h"

#include <string.h>

namespace Lazarus
{

SimpleXML::SimpleXML() {

    pthread_mutex_init(&m_mutex,NULL);
    m_is_open = false;
    m_doc = NULL;
    m_root_element = NULL;
    m_depth = 0;
    m_stop_search = false;
    m_sibling_counter = 0;
    m_current_node = NULL;
    m_child_check_ok = false;

    m_ctxt = xmlNewParserCtxt();

    LIBXML_TEST_VERSION;

//    xmlInitParser();
    xmlInitParserCtxt(m_ctxt);

}

SimpleXML::~SimpleXML() {

	if(m_doc != NULL)
	{
		xmlFreeDoc(m_doc);
	}
    xmlFreeParserCtxt(m_ctxt);

}

void SimpleXML::init()
{
	xmlInitParser();
}

void SimpleXML::free()
{
    xmlCleanupParser();
}

std::string SimpleXML::retrieveXMLString()
{
	std::string xml_doc;

	xmlChar* s = NULL;
	int size = 0;
	xmlDocDumpMemory(m_doc, &s, &size);

	if(s != NULL)
	{
		xml_doc = std::string((const char*)s);
	}

	xmlFree(s);

	return xml_doc;
}

int SimpleXML::parseFile(const std::string &filename)
{
    pthread_mutex_lock(&m_mutex);

    //free any existing data
    if(m_doc != NULL)
	{
		xmlFreeDoc(m_doc);
		xmlFreeParserCtxt(m_ctxt);
		xmlCleanupParser();

		m_ctxt = xmlNewParserCtxt();
		LIBXML_TEST_VERSION;
		xmlInitParserCtxt(m_ctxt);
	}

    //m_xml_text_reader = xmlReaderForFile(filename.c_str(), NULL, 0);
    m_doc = xmlCtxtReadFile(m_ctxt,filename.c_str(), NULL, 0);

    if(m_doc == NULL)
    {
        printf("ERROR: could not open xml file\n");

        pthread_mutex_unlock(&m_mutex);

        return -1;
    }

    m_filename = filename;
    m_root_element = xmlDocGetRootElement(m_doc);
    m_current_node = m_root_element;

    pthread_mutex_unlock(&m_mutex);

    return 0;

}

int SimpleXML::createEmptyTree(const std::string& root_node)
{
	//free any existing data
	if(m_doc != NULL)
	{
		xmlFreeDoc(m_doc);
		xmlFreeParserCtxt(m_ctxt);
		xmlCleanupParser();

		m_ctxt = xmlNewParserCtxt();
		LIBXML_TEST_VERSION;
		xmlInitParserCtxt(m_ctxt);
	}

	m_doc = xmlNewDoc(BAD_CAST "1.0");
	m_root_element = xmlNewNode(NULL, BAD_CAST root_node.c_str());
	xmlDocSetRootElement(m_doc, m_root_element);

	return 0;
}

int SimpleXML::nextNode()
{

    pthread_mutex_lock(&m_mutex);

    if(m_current_node->next == NULL)
    {
        printf("ERROR: no remaining siblings\n");

        pthread_mutex_unlock(&m_mutex);

        return -1;
    }
    else
    {
        m_current_node = m_current_node->next;

        pthread_mutex_unlock(&m_mutex);

        return 0;
    }


}

xmlNodePtr SimpleXML::get_m_current_node()
{
	return m_current_node;
}

void SimpleXML::reset_current_node()
{
	m_current_node = m_root_element;
}

int SimpleXML::childNode()
{

    pthread_mutex_lock(&m_mutex);

    if(m_current_node->children == NULL)
    {
        printf("ERROR: current node has no children\n");

        pthread_mutex_unlock(&m_mutex);

        return -1;
    }
    else
    {
        m_current_node = m_current_node->children;

        pthread_mutex_unlock(&m_mutex);

        return 0;
    }


}

void SimpleXML::printTabs(unsigned int count)
{
    for(unsigned int i=0;i<count;i++)
    {
        printf("\t");
    }
}

int SimpleXML::findNode(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, unsigned int sibling_nr)
{
	int result;

	//result = findNode_(path, start_node,node_name, node, sibling_nr); //iterates also over all siblings in the same layer
	result = findNode_(path, start_node->children,node_name, node, sibling_nr);//true depth search from the given node

	//reset the counters
	m_sibling_counter = 0;
	m_stop_search = false;

	return result;
}

int SimpleXML::findNode_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, unsigned int sibling_nr)
{
	//this initial value ensures that for an empty path vector path_ok will always equal true
    bool path_ok = true;

    //check all siblings
    for(xmlNodePtr current_node = start_node; current_node != NULL; current_node = current_node->next)
    {
        //check only element nodes
        if (current_node->type == XML_ELEMENT_NODE)
        {
            //only check the desired nodes (checks the name!)
            if( xmlStrcmp(current_node->name,(const xmlChar*)node_name.c_str()) == 0 )
            {

                //additionally check the hierarchy (iterate over path vector)
                xmlNodePtr parent_node = current_node->parent;

                for(std::vector<std::string>::reverse_iterator it = path.rbegin(); it != path.rend(); ++it)
                {
                    //premature end has been reached, i.e. the current depth within the tree is not deep enough
                    if(parent_node == NULL)
                    {
                        path_ok = false;
                        break;
                    }
                    else
                    {
                        //check the name of the parent with the given hierarchy
                        if( xmlStrcmp(parent_node->name,(const xmlChar*)((std::string)*it).c_str()) != 0 )
                        {
                            path_ok = false;
                            break;
                        }
                    }

                    parent_node = parent_node->parent;

                }

                //if path checks out for the first (or sibling_nr-nd) time within the recursion!!!(look below)
                if(path_ok == true && m_stop_search == false && m_sibling_counter == sibling_nr)
                {//printf("m_sibling_counter %d, sibling_nr %d\n",m_sibling_counter,sibling_nr);
                    //grab the node
                    node = current_node;
                    m_stop_search = true;
                    return 0;
                }

                //one sibling with the correct name has been found (but only start counting if indicated by sibling_nr>0)
				if(sibling_nr != 0 && path_ok==true)
				{
					m_sibling_counter++;
				}
            }


        }

        if(m_stop_search == false)
        {
            findNode_(path, current_node->children, node_name, node,sibling_nr);

            //check if the value has been found (esp. important for the top element on our call stack)
            if(m_stop_search == true)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }

    }

    return -1;
}


int SimpleXML::findNodeByChildnodesContent(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string childcontent)
{
	int result;

	result = findNodeByChildnodesContent_(path, start_node->children,node_name, node, childname, childcontent);

	//reset the indicator
	m_child_check_ok = false;
	m_stop_search = false;

	return result;
}

int SimpleXML::findNodeByChildnodesContent_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string childcontent)
{
	//this initial value ensures that for an empty path vector path_ok will always equal true
    bool path_ok = true;

    //determine if a search for childnodes content is required
    if(childname.compare("") == 0 )
    {
    	m_child_check_ok = true;
    }

    //check all siblings
    for(xmlNodePtr current_node = start_node; current_node != NULL; current_node = current_node->next)
    {
        //check only element nodes
        if (current_node->type == XML_ELEMENT_NODE)
        {
            //only check the desired nodes (checks the name!)
            if( xmlStrcmp(current_node->name,(const xmlChar*)node_name.c_str()) == 0 )
            {

                //---------- additionally check the hierarchy (iterate over path vector)
                xmlNodePtr parent_node = current_node->parent;

                for(std::vector<std::string>::reverse_iterator it = path.rbegin(); it != path.rend(); ++it)
                {
                    //premature end has been reached, i.e. the current depth within the tree is not deep enough
                    if(parent_node == NULL)
                    {
                        path_ok = false;
                        break;
                    }
                    else
                    {
                        //check the name of the parent with the given hierarchy
                        if( xmlStrcmp(parent_node->name,(const xmlChar*)((std::string)*it).c_str()) != 0 )
                        {
                            path_ok = false;
                            break;
                        }
                    }

                    parent_node = parent_node->parent;

                }

                //---------- also check the child for the specified content
                xmlNodePtr child = current_node->children;

                //iterate over the children (if required)
                std::string current_child_content;
                if(m_child_check_ok == false)
                {
					while(child != NULL)
					{
						if(child->type == XML_ELEMENT_NODE)
						{
							//only check the child with the specified name
							if( xmlStrcmp(child->name,(const xmlChar*)childname.c_str()) == 0 )
							{
								//check the content of the child with the given string
								getTextContent(child,current_child_content);
								if( current_child_content.compare(childcontent) == 0 )
								{
									m_child_check_ok = true;
									break;
								}
							}
						}

						child = child->next;//next child sibling
					}
                }

                //---------- if path checks out within the recursion!!!(look below) AND if the specified child has the given content
                if(path_ok == true && m_stop_search == false && m_child_check_ok == true)
                {
                    //grab the node
                    node = current_node;
                    m_stop_search = true;
                    return 0;
                }

            }


        }

        if(m_stop_search == false)
        {
        	findNodeByChildnodesContent_(path, current_node->children, node_name, node,childname,childcontent);

            //check if the value has been found (esp. important for the top element on our call stack)
            if(m_stop_search == true)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }

    }

    return -1;
}


int SimpleXML::findNodeByChildnodesAttribute(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string child_attribute)
{
	int result;

	result = findNodeByChildnodesAttribute_(path, start_node->children,node_name, node, childname, child_attribute);

	//reset the indicator
	m_child_check_ok = false;
	m_stop_search = false;

	return result;
}

int SimpleXML::findNodeByChildnodesAttribute_(std::vector<std::string> &path, xmlNodePtr start_node,const std::string &node_name,xmlNodePtr &node, std::string childname, std::string child_attribute)
{
	//this initial value ensures that for an empty path vector path_ok will always equal true
    bool path_ok = true;

    //determine if a search for childnodes content is required
    if(childname.compare("") == 0 )
    {
    	m_child_check_ok = true;
    }

    //check all siblings
    for(xmlNodePtr current_node = start_node; current_node != NULL; current_node = current_node->next)
    {
        //check only element nodes
        if (current_node->type == XML_ELEMENT_NODE)
        {
            //only check the desired nodes (checks the name!)
            if( xmlStrcmp(current_node->name,(const xmlChar*)node_name.c_str()) == 0 )
            {

                //---------- additionally check the hierarchy (iterate over path vector)
                xmlNodePtr parent_node = current_node->parent;

                for(std::vector<std::string>::reverse_iterator it = path.rbegin(); it != path.rend(); ++it)
                {
                    //premature end has been reached, i.e. the current depth within the tree is not deep enough
                    if(parent_node == NULL)
                    {
                        path_ok = false;
                        break;
                    }
                    else
                    {
                        //check the name of the parent with the given hierarchy
                        if( xmlStrcmp(parent_node->name,(const xmlChar*)((std::string)*it).c_str()) != 0 )
                        {
                            path_ok = false;
                            break;
                        }
                    }

                    parent_node = parent_node->parent;

                }

                //---------- also check the child for the specified content
                xmlNodePtr child = current_node->children;

                //iterate over the children (if required)
                std::string current_child_attribute;
                if(m_child_check_ok == false)
                {
					while(child != NULL)
					{
						if(child->type == XML_ELEMENT_NODE)
						{
							//only check the child with the specified name
							if( xmlStrcmp(child->name,(const xmlChar*)childname.c_str()) == 0 )
							{
								//check the content of the child with the given string
								getNodeAttribute(child,child_attribute,current_child_attribute);
								if( current_child_attribute.compare(child_attribute) == 0 )
								{
									m_child_check_ok = true;
									break;
								}
							}
						}

						child = child->next;//next child sibling
					}
                }

                //---------- if path checks out within the recursion!!!(look below) AND if the specified child has the given content
                if(path_ok == true && m_stop_search == false && m_child_check_ok == true)
                {
                    //grab the node
                    node = current_node;
                    m_stop_search = true;
                    return 0;
                }

            }


        }

        if(m_stop_search == false)
        {
        	findNodeByChildnodesContent_(path, current_node->children, node_name, node,childname,child_attribute);

            //check if the value has been found (esp. important for the top element on our call stack)
            if(m_stop_search == true)
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }

    }

    return -1;
}

void SimpleXML::listNodeAttributes__(xmlNodePtr node)
{
    //get details about the current node
    xmlChar* node_name = (xmlChar*)node->name;
    int type = node->type;

    printTabs(m_depth-1);
    printf("Node name: %s of type %d\n",node_name,type);


    //in case of text node output the content
    if (node->type == XML_TEXT_NODE)
    {
        printTabs(m_depth);
        xmlChar* content = xmlNodeListGetString(m_doc,node,1);
        printf("Content : '%s' of length %d\n",content,xmlStrlen(content));
        xmlFree(content);
    }


    //get the current nodes attributes and list their details
    xmlAttrPtr attribute = node->properties;
    xmlChar* attribute_name = NULL;
    xmlChar* attribute_value = NULL;

    while( attribute != NULL )
    {
        //read the attribute value
        attribute_name = (xmlChar*)attribute->name;
        attribute_value = attribute->children->content;

        printTabs(m_depth);
        printf("Attribute name: %s\n",attribute_name);
        printTabs(m_depth);
        printf("Value : %s\n",attribute_value);

        attribute = attribute->next;

    }

}

void SimpleXML::listNodeAttributes(xmlNodePtr node)
{
    pthread_mutex_lock(&m_mutex);

    //get details about the current node
    xmlChar* node_name = (xmlChar*)node->name;
    int type = node->type;

    printf("Node name: %s of type %d with content %s\n",node_name,type,(xmlChar*)node->content);

    //get the current nodes attributes and list their details
    xmlAttrPtr attribute = node->properties;
    xmlChar* attribute_name = NULL;
    xmlChar* attribute_value = NULL;

    while( attribute != NULL )
    {
        //read the attribute value
        attribute_name = (xmlChar*)attribute->name;
        attribute_value = attribute->children->content;

        printf("Attribute name: %s\n",attribute_name);
        printf("Value : %s\n",attribute_value);

        attribute = attribute->next;

    }

    pthread_mutex_unlock(&m_mutex);

}

xmlNodePtr SimpleXML::getRootElement()
{
    return m_root_element;
}

void SimpleXML::printXMLTree(xmlNodePtr node)
{
    m_depth++;

    for(xmlNodePtr current_node = node; current_node != NULL; current_node = current_node->next)
    {
        //if (current_node->type == XML_ELEMENT_NODE)
        {
            listNodeAttributes__(current_node);
        }

        printXMLTree(current_node->children);
    }

    m_depth--;

}

int SimpleXML::getNodeAttribute(xmlNodePtr node,const std::string &attribute, std::string &value)
{
    xmlAttrPtr attribute_ = node->properties;
    xmlChar* attribute_name = NULL;
    xmlChar* attribute_value = NULL;

    while( attribute_ != NULL )
    {
        //search for the desired attribute
        attribute_name = (xmlChar*)attribute_->name;

        if( xmlStrcmp(attribute_name,(xmlChar*)attribute.c_str()) == 0 )
        {

            attribute_value = xmlNodeListGetString(m_doc,attribute_->children,1);

            value = std::string( (char*)attribute_value );
            xmlFree(attribute_value);
            return 0;
        }

        xmlFree(attribute_value);

        attribute_ = attribute_->next;

    }

    return -1;
}

int SimpleXML::setNodeAttribute(xmlNodePtr node,const std::string &attribute, const std::string &value)
{
    xmlAttrPtr attribute_ = node->properties;
    xmlChar* attribute_name = NULL;

    while( attribute_ != NULL )
    {
        //find the attribute and set its value
        attribute_name = (xmlChar*)attribute_->name;

        if( xmlStrcmp(attribute_name,(xmlChar*)attribute.c_str()) == 0 )
        {
            xmlNodeSetContent(attribute_->children,(xmlChar*)value.c_str());
            return 0;
        }

        attribute_ = attribute_->next;

    }

    return -1;
}

int SimpleXML::removeNodeAttribute_(xmlNodePtr node,const std::string &attribute)
{
    xmlAttrPtr attribute_ = node->properties;
    xmlChar* attribute_name = NULL;

    while( attribute_ != NULL )
    {
        //find the attribute and remove it
        attribute_name = (xmlChar*)attribute_->name;

        if( xmlStrcmp(attribute_name,(xmlChar*)attribute.c_str()) == 0 )
        {
            xmlRemoveProp(attribute_);
            return 0;
        }

        attribute_ = attribute_->next;

    }

    return -1;
}

int SimpleXML::readNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, std::string &value, unsigned int sibling_nr)
{

    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //grab the attribute
    result = getNodeAttribute(node, attribute, value);

    pthread_mutex_unlock(&m_mutex);

    return result;
}


int SimpleXML::readNodeContent(std::vector<std::string> &path,const std::string &node_name, std::string &value, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //grab the content
    result = getTextContent(node, value);

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::getTextContent(xmlNodePtr node, std::string &value)
{
    xmlNodePtr child = node->children;
    xmlChar* content = NULL;

    //search children for text nodes
    while( child != NULL )
    {
        if( child->type == XML_TEXT_NODE )
        {
            content = xmlNodeListGetString(m_doc,child,1);
            value = std::string( (char*)content );
            xmlFree(content);
            return 0;
        }

        child = child->next;

    }

    return -1;
}

int SimpleXML::setTextContent(xmlNodePtr node, const std::string &value)
{
    xmlNodePtr child = node->children;

    //search children for text nodes
    while( child != NULL )
    {
        if( child->type == XML_TEXT_NODE )
        {
            xmlNodeSetContent(child,(xmlChar*)value.c_str());
            return 0;
        }

        child = child->next;

    }

    return -1;
}

int SimpleXML::removeTextChild(xmlNodePtr node)
{
    xmlNodePtr child = node->children;

    //search children for text nodes
    while( child != NULL )
    {
        if( child->type == XML_TEXT_NODE )
        {
            xmlUnlinkNode(child);
            xmlFreeNode(child);
            return 0;
        }

        child = child->next;

    }

    return -1;
}

int SimpleXML::changeNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, const std::string &value, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //grab the content
    result = setNodeAttribute(node, attribute, value);

    pthread_mutex_unlock(&m_mutex);

    return result;
}


int SimpleXML::addNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, const std::string &value, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //add an attribute
    xmlNewProp(node,(xmlChar*)attribute.c_str(),(xmlChar*)value.c_str());
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::removeNodeAttribute(std::vector<std::string> &path,const std::string &node_name,const std::string &attribute, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //remove the attribute
    result = removeNodeAttribute_(node, attribute);

    pthread_mutex_unlock(&m_mutex);

    return result;
}


int SimpleXML::changeNodeContent(std::vector<std::string> &path,const std::string &node_name, const std::string &value, unsigned int sibling_nr)
{

    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //grab the content
    result = setTextContent(node, value);

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::addNodeContent(std::vector<std::string> &path,const std::string &node_name, const std::string &value, unsigned int sibling_nr)
{

    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //add the content
    xmlNodeAddContent(node,(xmlChar*)value.c_str());
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::removeNodeContent(std::vector<std::string> &path,const std::string &node_name, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //add the content
    removeTextChild(node);
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}


int SimpleXML::addNode(std::vector<std::string> &path,const std::string &node_name,const std::string &new_node_name, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //add the node as a child
    xmlNodePtr new_node = xmlNewNode(NULL,(xmlChar*)new_node_name.c_str());
    xmlAddChild(node,new_node);
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::addNodeSibling(std::vector<std::string> &path,const std::string &node_name,const std::string &new_node_name, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //add the node as a child
    xmlNodePtr new_node = xmlNewNode(NULL,(xmlChar*)new_node_name.c_str());
    xmlAddSibling(node,new_node);
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}

int SimpleXML::removeNode(std::vector<std::string> &path,const std::string &node_name, unsigned int sibling_nr)
{
    pthread_mutex_lock(&m_mutex);

    //search for the desired node
    xmlNodePtr node = NULL;
    int result = findNode(path,m_root_element, node_name,node,sibling_nr);

    if(result == -1)
    {
        //printf("node not found\n");
        pthread_mutex_unlock(&m_mutex);

        return result;
    }

    m_stop_search = false;

    //remove the node
    xmlUnlinkNode(node);
    xmlFreeNode(node);
    result = 0;

    pthread_mutex_unlock(&m_mutex);

    return result;
}


int SimpleXML::saveXMLTree(const std::string& filename)
{
    return xmlSaveFormatFile(filename.c_str(), m_doc,1);
}

}
