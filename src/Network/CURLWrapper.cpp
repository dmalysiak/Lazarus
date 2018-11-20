/*
 * CURLWrapper.cpp
 *
 *  Created on: 29.12.2014
 *      Author: Darius Malysiak
 */

#include "CURLWrapper.h"
#include "../DataStructures/FastNCSList.h"
#include "../Toolbox.h"
#include "../PrimitiveBuffer.h"

#include <string.h>

namespace Lazarus {

#ifdef USECURL

struct bufferCallbackReadHelper
{
	Lazarus::BasicBuffer* data;
	unsigned int read_bytes;
};

CURLWrapper::CURLWrapper() {

	curl_global_init(CURL_GLOBAL_ALL);

}

CURLWrapper::~CURLWrapper() {

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
}

CURLWrapper* CURLWrapper::getInstance()
{
	if(mp_instance == NULL)
	{
		mp_instance = new CURLWrapper();
	}

	return mp_instance;
}

size_t CURLWrapper::writeMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	Lazarus::BasicBuffer* buffer = (Lazarus::BasicBuffer*)userp;

	//we start with an empty buffer (i.e. length 0) and increase it just as much as we need
	unsigned int old_length = buffer->get_m_length();
	buffer->changeSize(old_length + size*nmemb);

	//copy the new data
	memcpy(buffer->get_mp_data()+old_length,contents,size*nmemb);

	//normally one should check for errors by returning the actual amount of bytes (i.e. not the desired one).
	//CURL would abort the transfer if the returned value wouldn't equal 'size*nmemb'!
	return size*nmemb;
}

size_t CURLWrapper::readMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	struct bufferCallbackReadHelper* helper = (struct bufferCallbackReadHelper*)userp;

	//how much data can we give in this iteration?
	unsigned int bytes_desired = std::min(helper->data->get_m_length() - helper->read_bytes, (unsigned int)( size*nmemb) );

	//copy the new data
	memcpy(contents,helper->data->get_mp_data()+helper->read_bytes,bytes_desired);
	helper->read_bytes += bytes_desired;

	//normally one should check for errors by returning the actual amount of bytes (i.e. not the desired one).
	//CURL would abort the transfer if the returned value wouldn't equal 'size*nmemb'!
	return bytes_desired;
}

Lazarus::BasicBuffer* CURLWrapper::getHTTP(const std::string& url)
{
	/* init the curl session */
	CURL* c_handle = curl_easy_init();

    //A buffer which will grow!
    Lazarus::BasicBuffer* output = new Lazarus::BasicBuffer();

	/* specify URL to get */
    curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

    /* send all data to this function  */
    curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

    /* we pass our buffer to the callback function */
    curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)output);

    /* no progress meter please */
    curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

    /* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
    curl_easy_setopt(c_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");


    /* get it! */
    CURLcode res = curl_easy_perform(c_handle);

    /* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete output;
		output = NULL;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);

	return output;
}

int CURLWrapper::getHTTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	*payload = new Lazarus::BasicBuffer();
	*http_header = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)*payload);

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_HEADERDATA, (void *)*http_header);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
	curl_easy_setopt(c_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete *payload;
		delete *http_header;
		*payload = NULL;
		*http_header = NULL;
		error = -1;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);


	return error;
}

Lazarus::BasicBuffer* CURLWrapper::getHTTPS(const std::string& url)
{
	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	Lazarus::BasicBuffer* output = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)output);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* also accept connection if the certificates hostname differs from the actual one */
	curl_easy_setopt(c_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	/* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
	curl_easy_setopt(c_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete output;
		output = NULL;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);

	return output;
}


int CURLWrapper::getHTTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	*payload = new Lazarus::BasicBuffer();
	*http_header = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* also accept connection if the certificates hostname differs from the actual one */
	curl_easy_setopt(c_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)*payload);

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_HEADERDATA, (void *)*http_header);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* some servers don't like requests that are made without a user-agent
	 field, so we provide one */
	curl_easy_setopt(c_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete *payload;
		delete *http_header;
		*payload = NULL;
		*http_header = NULL;
		error = -1;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);


	return error;
}

Lazarus::BasicBuffer* CURLWrapper::getFTP(const std::string& url)
{
	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	Lazarus::BasicBuffer* output = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)output);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete output;
		output = NULL;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);

	return output;
}


int CURLWrapper::getFTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	*payload = new Lazarus::BasicBuffer();
	*ftp_header = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)*payload);

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_HEADERDATA, (void *)*ftp_header);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete *payload;
		delete *ftp_header;
		*payload = NULL;
		*ftp_header = NULL;
		error = -1;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);


	return error;
}


Lazarus::BasicBuffer* CURLWrapper::getFTPS(const std::string& url)
{
	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	Lazarus::BasicBuffer* output = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)output);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* We activate SSL and we require it for both control and data */
	curl_easy_setopt(c_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

	/* Switch on full protocol/debug output */
	curl_easy_setopt(c_handle, CURLOPT_VERBOSE, 1L);

	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete output;
		output = NULL;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);

	return output;
}


int CURLWrapper::getFTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	*payload = new Lazarus::BasicBuffer();
	*ftp_header = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)*payload);

	/* we pass our payload buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_HEADERDATA, (void *)*ftp_header);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* We activate SSL and we require it for both control and data */
	curl_easy_setopt(c_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

	/* Switch on full protocol/debug output */
	curl_easy_setopt(c_handle, CURLOPT_VERBOSE, 1L);

	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete *payload;
		delete *ftp_header;
		*payload = NULL;
		*ftp_header = NULL;
		error = -1;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);


	return error;
}

int CURLWrapper::putFTP(const std::string& url,Lazarus::BasicBuffer* data)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	if(c_handle == NULL)
	{
		return -1;
	}

	struct bufferCallbackReadHelper helper;
	helper.data = data;
	helper.read_bytes = 0;

	/* we want to use our own read function */
	curl_easy_setopt(c_handle, CURLOPT_READFUNCTION, &(CURLWrapper::readMemoryCallback) );

	/* enable uploading */
	curl_easy_setopt(c_handle, CURLOPT_UPLOAD, 1L);

	/* specify target */
	curl_easy_setopt(c_handle,CURLOPT_URL, url.c_str());

	/* now specify which file to upload */
	curl_easy_setopt(c_handle, CURLOPT_READDATA, &helper);

	/* Set the size of the file to upload (optional).  If you give a *_LARGE
	   option you MUST make sure that the type of the passed-in argument is a
	   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
	   make sure that to pass in a type 'long' argument. */
	curl_easy_setopt(c_handle, CURLOPT_INFILESIZE_LARGE,
					 (curl_off_t)data->get_m_length());

	/* Now run off and do what you've been told! */
	CURLcode res = curl_easy_perform(c_handle);

	/* Check for errors */
	if(res != CURLE_OK)
	{
	  printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	  error = -1;
	}

	/* always cleanup */
	curl_easy_cleanup(c_handle);

	return error;
}

int CURLWrapper::putFTPS(const std::string& url,Lazarus::BasicBuffer* data)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	if(c_handle == NULL)
	{
		return -1;
	}

	struct bufferCallbackReadHelper helper;
	helper.data = data;
	helper.read_bytes = 0;

	/* we want to use our own read function */
	curl_easy_setopt(c_handle, CURLOPT_READFUNCTION, &(CURLWrapper::readMemoryCallback) );

	/* enable uploading */
	curl_easy_setopt(c_handle, CURLOPT_UPLOAD, 1L);

	/* specify target */
	curl_easy_setopt(c_handle,CURLOPT_URL, url.c_str());

	/* now specify which file to upload */
	curl_easy_setopt(c_handle, CURLOPT_READDATA, &helper);

	/* Set the size of the file to upload (optional).  If you give a *_LARGE
	   option you MUST make sure that the type of the passed-in argument is a
	   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
	   make sure that to pass in a type 'long' argument. */
	curl_easy_setopt(c_handle, CURLOPT_INFILESIZE_LARGE,
					 (curl_off_t)data->get_m_length());


	/* We activate SSL and we require it for both control and data */
	curl_easy_setopt(c_handle, CURLOPT_USE_SSL, CURLUSESSL_ALL);

	/* Switch on full protocol/debug output */
	curl_easy_setopt(c_handle, CURLOPT_VERBOSE, 1L);

	/* Now run off and do what you've been told! */
	CURLcode res = curl_easy_perform(c_handle);

	/* Check for errors */
	if(res != CURLE_OK)
	{
	  printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	  error = -1;
	}

	/* always cleanup */
	curl_easy_cleanup(c_handle);

	return error;
}


Lazarus::BasicBuffer* CURLWrapper::getSCP(const std::string& url, const std::string& username, const std::string& password)
{
	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	//A buffer which will grow!
	Lazarus::BasicBuffer* output = new Lazarus::BasicBuffer();

	/* specify URL to get */
	curl_easy_setopt(c_handle, CURLOPT_URL, url.c_str());

	/* send all data to this function  */
	curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, &(CURLWrapper::writeMemoryCallback) );

	/* we pass our buffer to the callback function */
	curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)output);

	/* no progress meter please */
	curl_easy_setopt(c_handle, CURLOPT_NOPROGRESS, 1L);

	/* adjust user and password */
	curl_easy_setopt(c_handle, CURLOPT_USERPWD, (username+password).c_str());


	/* get it! */
	CURLcode res = curl_easy_perform(c_handle);

	/* check for errors */
	if(res != CURLE_OK)
	{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		delete output;
		output = NULL;
	}

	/* cleanup curl stuff */
	curl_easy_cleanup(c_handle);

	return output;
}

int CURLWrapper::putSCP(const std::string& url, Lazarus::BasicBuffer* data, const std::string& username, const std::string& password)
{
	int error = 0;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	if(c_handle == NULL)
	{
		return -1;
	}

	struct bufferCallbackReadHelper helper;
	helper.data = data;
	helper.read_bytes = 0;

	/* we want to use our own read function */
	curl_easy_setopt(c_handle, CURLOPT_READFUNCTION, &(CURLWrapper::readMemoryCallback) );

	/* enable uploading */
	curl_easy_setopt(c_handle, CURLOPT_UPLOAD, 1L);

	/* specify target */
	curl_easy_setopt(c_handle,CURLOPT_URL, url.c_str());

	/* now specify which file to upload */
	curl_easy_setopt(c_handle, CURLOPT_READDATA, &helper);

	/* adjust user and password */
	curl_easy_setopt(c_handle, CURLOPT_USERPWD, (username+password).c_str());

	/* Set the size of the file to upload (optional).  If you give a *_LARGE
	   option you MUST make sure that the type of the passed-in argument is a
	   curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
	   make sure that to pass in a type 'long' argument. */
	curl_easy_setopt(c_handle, CURLOPT_INFILESIZE_LARGE,
					 (curl_off_t)data->get_m_length());

	/* Now run off and do what you've been told! */
	CURLcode res = curl_easy_perform(c_handle);

	/* Check for errors */
	if(res != CURLE_OK)
	{
	  printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

	  error = -1;
	}

	/* always cleanup */
	curl_easy_cleanup(c_handle);

	return error;
}



int CURLWrapper::send_SMTP_TLS_Message(const std::string& message, const std::string& subject,
		const std::string& username, const std::string& password,
			const std::string& recipients, const std::string& cc_recipients, const std::string& bcc_recipients,
			const std::string& from,
			const std::string& server_ip, unsigned int server_port)
{
	CURLcode res = CURLE_OK;
	struct curl_slist* recipients_ = NULL;

	/* init the curl session */
	CURL* c_handle = curl_easy_init();

	if(c_handle == NULL)
	{
		return -1;
	}

	/* Set username and password */
	curl_easy_setopt(c_handle, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(c_handle, CURLOPT_PASSWORD, password.c_str());

	/*Server address*/
	curl_easy_setopt(c_handle, CURLOPT_URL,
			(std::string("smtp://")+server_ip+std::string(":")+Lazarus::Toolbox::uintToString(server_port)).c_str());

	/*Use SSL*/
	curl_easy_setopt(c_handle, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

	/*Who has sent the mail?*/
	curl_easy_setopt(c_handle, CURLOPT_MAIL_FROM, from.c_str());

	/*The recipients*/
	Lazarus::FastNCSList<std::string>* recips = Lazarus::Toolbox::splitStringF(recipients,';');
	Lazarus::FastNCSList<std::string>* recips_cc = Lazarus::Toolbox::splitStringF(cc_recipients,';');
	Lazarus::FastNCSList<std::string>* recips_bcc = Lazarus::Toolbox::splitStringF(bcc_recipients,';');

	for(auto it = recips->getStart();it!=recips->getEnd();++it)
	{
		recipients_ = curl_slist_append(recipients_, it.getElement()->getm_data_ref().c_str());
	}

	for(auto it = recips_cc->getStart();it!=recips_cc->getEnd();++it)
	{
		recipients_ = curl_slist_append(recipients_, it.getElement()->getm_data_ref().c_str());
	}

	for(auto it = recips_bcc->getStart();it!=recips_bcc->getEnd();++it)
	{
		recipients_ = curl_slist_append(recipients_, it.getElement()->getm_data_ref().c_str());
	}

	delete(recips);

	curl_easy_setopt(c_handle, CURLOPT_MAIL_RCPT, recipients_);

	/*the message*/
	std::string assembled = constructMailHeader(recipients, cc_recipients, bcc_recipients, from, subject, message);

	struct bufferCallbackReadHelper helper;
	helper.data = new Lazarus::PrimitiveBuffer((unsigned char*)assembled.c_str(),assembled.length()+1);
	helper.read_bytes = 0;

	/* we want to use our own read function */
	curl_easy_setopt(c_handle, CURLOPT_READFUNCTION, &(CURLWrapper::readMemoryCallback) );
	curl_easy_setopt(c_handle, CURLOPT_READDATA, &helper);
	curl_easy_setopt(c_handle, CURLOPT_UPLOAD, 1L);

	/* Send the message */
	res = curl_easy_perform(c_handle);

	/* Check for errors */
	if(res != CURLE_OK)
	  fprintf(stderr, "curl_easy_perform() failed: %s\n",
			  curl_easy_strerror(res));

	/* Free the list of recipients */
	curl_slist_free_all(recipients_);

	/* Always cleanup */
	curl_easy_cleanup(c_handle);

	delete helper.data;

	return 0;
}

std::string CURLWrapper::constructMailHeader(const std::string& recipients, const std::string& cc,
			const std::string& bcc,const std::string& from, const std::string& subject, const std::string& message)
{
	std::string out = "To: " +recipients+ "\r\n";
	out += "From: " +from+ "\r\n";
	out += "Cc: " +cc+ "\r\n";
	out += "Bcc: " +bcc+ "\r\n";
	out += "Subject:" +subject+ "\r\n";
	out += "\r\n";
	out += message;
	out += "\r\n";

	return out;
}

#endif

#ifdef NOCURL

struct bufferCallbackReadHelper
{
	Lazarus::BasicBuffer* data;
	unsigned int read_bytes;
};

CURLWrapper::CURLWrapper() {
}

CURLWrapper::~CURLWrapper() {
}

CURLWrapper* CURLWrapper::getInstance()
{
	if(mp_instance == NULL)
	{
		mp_instance = new CURLWrapper();
	}

	return mp_instance;
}

size_t CURLWrapper::writeMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	return 0;
}

size_t CURLWrapper::readMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	return 0;
}

Lazarus::BasicBuffer* CURLWrapper::getHTTP(const std::string& url)
{
	return 0;
}

int CURLWrapper::getHTTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header)
{
	return 0;
}

Lazarus::BasicBuffer* CURLWrapper::getHTTPS(const std::string& url)
{
	return 0;
}


int CURLWrapper::getHTTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header)
{
	return 0;
}

Lazarus::BasicBuffer* CURLWrapper::getFTP(const std::string& url)
{
	return 0;
}


int CURLWrapper::getFTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header)
{
	return 0;
}


Lazarus::BasicBuffer* CURLWrapper::getFTPS(const std::string& url)
{
	return 0;
}


int CURLWrapper::getFTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header)
{
	return 0;
}

int CURLWrapper::putFTP(const std::string& url,Lazarus::BasicBuffer* data)
{
	return 0;
}

int CURLWrapper::putFTPS(const std::string& url,Lazarus::BasicBuffer* data)
{
	return 0;
}


Lazarus::BasicBuffer* CURLWrapper::getSCP(const std::string& url, const std::string& username, const std::string& password)
{
	return 0;
}

int CURLWrapper::putSCP(const std::string& url, Lazarus::BasicBuffer* data, const std::string& username, const std::string& password)
{
	return 0;
}



int CURLWrapper::send_SMTP_TLS_Message(const std::string& message, const std::string& subject,
		const std::string& username, const std::string& password,
			const std::string& recipients, const std::string& cc_recipients, const std::string& bcc_recipients,
			const std::string& from,
			const std::string& server_ip, unsigned int server_port)
{
	return 0;
}

std::string CURLWrapper::constructMailHeader(const std::string& recipients, const std::string& cc,
			const std::string& bcc,const std::string& from, const std::string& subject, const std::string& message)
{
	std::string out = "To: " +recipients+ "\r\n";
	out += "From: " +from+ "\r\n";
	out += "Cc: " +cc+ "\r\n";
	out += "Bcc: " +bcc+ "\r\n";
	out += "Subject:" +subject+ "\r\n";
	out += "\r\n";
	out += message;
	out += "\r\n";

	return out;
}

#endif

CURLWrapper* CURLWrapper::mp_instance = NULL;

} /* namespace Lazarus */
