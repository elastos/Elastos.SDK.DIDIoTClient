#ifndef _HTTP_CLIENT_HPP_
#define _HTTP_CLIENT_HPP_

#include <map>
#include <sstream>
#include <string>
#include <vector>

struct curl_slist;

namespace elastos {

class HttpClient {
public:
	/*** type define ***/
	class ErrCode {
	public:
		static constexpr int Unknown        = -1;
		static constexpr int NotFound       = -2;
		static constexpr int NullArgument   = -3;
		static constexpr int BadArgument    = -4;
		static constexpr int UrlNotExists   = -5;
		static constexpr int UserCanceled   = -6;
		static constexpr int IOFailed       = -7;
		static constexpr int NetFailed      = -8;
		static constexpr int CurlBaseCode   = -500;
	private:
		explicit ErrCode() = delete;
		virtual ~ErrCode() = delete;
	};

	using HeaderValue = std::vector<std::string>;
	using HeaderMap   = std::map<std::string, HeaderValue>;

	/*** static function and variable ***/
	static int InitGlobal();

  /*** class function and variable ***/
	explicit HttpClient();
	virtual ~HttpClient();

	int Url(const std::string& url);
	int AddHeader(const std::string& name, const std::string& value);
	int SetHeader(const std::string& name, const std::string& value);
	int SetTimeout(unsigned long milliSecond);

	int SyncGet();
	int SyncPost(const int8_t* body, int size);
	int SyncPost(const std::string& body);

	int GetResponseStatus() const;
	int GetResponseReason(std::string& message) const;
	int GetResponseHeaders(HeaderMap& headers) const;
	int GetResponseHeader(const std::string& name, HeaderValue& value) const;
	int GetResponseBody(std::shared_ptr<int8_t>& body);
	int GetResponseBody(std::string& body);

protected:
  /*** type define ***/

  /*** static function and variable ***/

  /*** class function and variable ***/

private:
  /*** type define ***/
	static constexpr const char* SCHEME_HTTP  = "http://";
	static constexpr const char* SCHEME_HTTPS = "https://";

  /*** static function and variable ***/
	static size_t CurlHeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);
	static size_t CurlWriteCallback(char* buffer, size_t size, size_t nitems, void* userdata);
	static size_t CurlReadCallback(char* buffer, size_t size, size_t nitems, void* userdata);

  /*** class function and variable ***/
	int MakeCurl(std::shared_ptr<void>& curlHandlePtr, std::shared_ptr<struct curl_slist>& curlHeadersPtr) const;
	int AddHeader(HeaderMap& headers,
				  const std::string& name, const std::string& value) const;

	std::string mUrl;
	long mTimeoutMS;
	HeaderMap mReqHeaders;

	int mRespStatus;
	std::string mRespReason;
	HeaderMap mRespHeaders;
	std::stringstream mRespBody;
};

/***********************************************/
/***** class template function implement *******/
/***********************************************/

/***********************************************/
/***** macro definition ************************/
/***********************************************/

} // namespace elastos

#endif /* _HTTP_CLIENT_HPP_ */

