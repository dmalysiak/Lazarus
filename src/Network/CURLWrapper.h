/*
 * CURLWrapper.h
 *
 *  Created on: 29.12.2014
 *      Author: Darius Malysiak
 */

#ifndef CURLWRAPPER_H_
#define CURLWRAPPER_H_

#include "../BaseObject.h"
#include "../BasicBuffer.h"

#ifdef USECURL
#include <curl/curl.h>
#endif

#include <string>

namespace Lazarus {

/**
 * A thread safe and object oriented wrapper class for basic CURL tasks.
 */
class CURLWrapper: public Lazarus::BaseObject {
public:
	virtual ~CURLWrapper();

	static CURLWrapper* getInstance();

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return NULL.
	 * Otherwise the fetched data is returned, which should be treated as binary data,
	 * e.g. not as a terminated c-string!
	 */
	Lazarus::BasicBuffer* getHTTP(const std::string& url);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return -1.
	 * Otherwise it will return 0. Additionally this method returns the HTTP header.
	 * The fetched data should be treated as binary data, e.g. not as a terminated c-string!
	 * The pointer-pointers must point to empty pointers (i.e. not pointing to an object).
	 * In case of errors the >pointers< will be set to NULL.
	 */
	int getHTTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return NULL.
	 * Otherwise the fetched data is returned, which should be treated as binary data,
	 * e.g. not as a terminated c-string!
	 */
	Lazarus::BasicBuffer* getHTTPS(const std::string& url);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return -1.
	 * Otherwise it will return 0. Additionally this method returns the HTTP header.
	 * The fetched data should be treated as binary data, e.g. not as a terminated c-string!
	 * The pointer-pointers must point to empty pointers (i.e. not pointing to an object).
	 * In case of errors the >pointers< will be set to NULL.
	 */
	int getHTTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** http_header);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return NULL.
	 * Otherwise the fetched data is returned, which should be treated as binary data,
	 * e.g. not as a terminated c-string!
	 * Note: in order to get an index of a FTP directory, just enter e.g. 'ftp://ftp.u-tx.net/archlinux/'.
	 * The output will be like:
	 * drwxr-xr-x    3 1000       100              4096 Jul 22  2009 community
	 * drwxr-xr-x    3 1000       100              4096 Sep 11  2010 community-staging
	 * drwxr-xr-x    3 1000       100              4096 Jul 16  2009 community-testing
	 */
	Lazarus::BasicBuffer* getFTP(const std::string& url);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return -1.
	 * Otherwise it will return 0. Additionally this method returns the FTP header.
	 * The fetched data should be treated as binary data, e.g. not as a terminated c-string!
	 * The pointer-pointers must point to empty pointers (i.e. not pointing to an object).
	 * In case of errors the >pointers< will be set to NULL.
	 * Note: in order to get an index of a FTP directory, just enter e.g. 'ftp://ftp.u-tx.net/archlinux/'.
	 * The output will be like:
	 * drwxr-xr-x    3 1000       100              4096 Jul 22  2009 community
	 * drwxr-xr-x    3 1000       100              4096 Sep 11  2010 community-staging
	 * drwxr-xr-x    3 1000       100              4096 Jul 16  2009 community-testing
	 */
	int getFTPwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return NULL.
	 * Otherwise the fetched data is returned, which should be treated as binary data,
	 * e.g. not as a terminated c-string!
	 * Note: in order to get an index of a FTP directory, just enter e.g. 'ftp://ftp.u-tx.net/archlinux/'.
	 * The output will be like:
	 * drwxr-xr-x    3 1000       100              4096 Jul 22  2009 community
	 * drwxr-xr-x    3 1000       100              4096 Sep 11  2010 community-staging
	 * drwxr-xr-x    3 1000       100              4096 Jul 16  2009 community-testing
	 */
	Lazarus::BasicBuffer* getFTPS(const std::string& url);

	/**
	 * Will fetch the data from the specified URL, in case of errors it will return -1.
	 * Otherwise it will return 0. Additionally this method returns the FTP header.
	 * The fetched data should be treated as binary data, e.g. not as a terminated c-string!
	 * The pointer-pointers must point to empty pointers (i.e. not pointing to an object).
	 * In case of errors the >pointers< will be set to NULL.
	 * Note: in order to get an index of a FTP directory, just enter e.g. 'ftp://ftp.u-tx.net/archlinux/'.
	 * The output will be like:
	 * drwxr-xr-x    3 1000       100              4096 Jul 22  2009 community
	 * drwxr-xr-x    3 1000       100              4096 Sep 11  2010 community-staging
	 * drwxr-xr-x    3 1000       100              4096 Jul 16  2009 community-testing
	 */
	int getFTPSwHeader(const std::string& url,Lazarus::BasicBuffer** payload, Lazarus::BasicBuffer** ftp_header);


	/**
	 * Will upload 'data' to the given URL (i.e. filename, "everything is a file"). Returns 0 on success,
	 * -1 otherwise. Existing files will be overwritten!
	 */
	int putFTP(const std::string& url,Lazarus::BasicBuffer* data);

	/**
	 * Will upload 'data' to the given URL (i.e. filename, "everything is a file"). Returns 0 on success,
	 * -1 otherwise. Existing files will be overwritten!
	 */
	int putFTPS(const std::string& url,Lazarus::BasicBuffer* data);

	/**
	 * Will fetch the data from the specified URL (e.g. "scp://<my_host>:<my_port>/home/lazarus/test.txt"),
	 * in case of errors it will return NULL.
	 * Otherwise the fetched data is returned, which should be treated as binary data,
	 * e.g. not as a terminated c-string!
	 */
	Lazarus::BasicBuffer* getSCP(const std::string& url, const std::string& username, const std::string& password);

	/**
	 * Will upload 'data' to the given URL (i.e. filename, "everything is a file")  (e.g.
	 * "scp://<my_host>:<my_port>/home/lazarus/test.txt"). Returns 0 on success,
	 * -1 otherwise. Existing files will be overwritten!
	 */
	int putSCP(const std::string& url, Lazarus::BasicBuffer* data, const std::string& username, const std::string& password);

	//--------------------- email methods

	/**
	 * Multiple entries in recipients, cc_receipients and bcc_receipients must be separated via ';'
	 * */
	int send_SMTP_TLS_Message(const std::string& message,const std::string& subject,
			const std::string& username, const std::string& password,
			const std::string& receipients, const std::string& cc_recipients, const std::string& bcc_receipients,
			const std::string& from,
			const std::string& server_ip,  unsigned int server_port);

private:
	CURLWrapper();
	static CURLWrapper* mp_instance;

	static size_t writeMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);
	static size_t readMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

	std::string constructMailHeader(const std::string& recipients, const std::string& cc,
			const std::string& bcc,const std::string& from,const std::string& subject, const std::string& message);
};

} /* namespace Lazarus */

#endif /* CURLWRAPPER_H_ */
