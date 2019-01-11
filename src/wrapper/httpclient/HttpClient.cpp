#include <HttpClient.hpp>

#include <iostream>

#include <curl/curl.h>
#include <Log.hpp>

#define CHECK_ERROR(curl_code) \
	if(curl_code != CURLE_OK) { \
		int errcode = (ErrCode::CurlBaseCode + (-curl_code)); \
		Log::E(Log::TAG, "Failed to call %s, return %d.", __PRETTY_FUNCTION__, errcode); \
		return errcode; \
	}

namespace elastos {

/* =========================================== */
/* === static variables initialize =========== */
/* =========================================== */


/* =========================================== */
/* === static function implement ============= */
/* =========================================== */
int HttpClient::InitGlobal()
{
	CURLcode curle;
	curle = curl_global_init(CURL_GLOBAL_ALL);
	CHECK_ERROR(curle);

	//curl_global_cleanup(); // never called

	return 0;
}

/* =========================================== */
/* === class public function implement  ====== */
/* =========================================== */
HttpClient::HttpClient()
	: mUrl()
	, mTimeoutMS(10000)
	, mReqHeaders()
	, mRespStatus(-1)
	, mRespReason()
	, mRespHeaders()
	, mRespBody()
{
}

HttpClient::~HttpClient()
{
}

int HttpClient::Url(const std::string& url)
{
	if(url.empty() == true) {
		return ErrCode::NullArgument;
	}

	if(url.compare(0, std::strlen(SCHEME_HTTP), SCHEME_HTTP) != 0
	&& url.compare(0, std::strlen(SCHEME_HTTPS), SCHEME_HTTPS) != 0) {
		return ErrCode::BadArgument;
	}

	mUrl = url;

	return 0;
}

int HttpClient::AddHeader(const std::string& name, const std::string& value)
{
	int ret = 0;
	
	ret = AddHeader(mReqHeaders, name, value);

	return ret;
}

int HttpClient::SetHeader(const std::string& name, const std::string& value)
{
	int ret = 0;
	if(name.empty() == true) {
		return ErrCode::NullArgument;
	}

	auto found = mReqHeaders.find(name);
	if(found != mReqHeaders.end()) {
		found->second.clear();
	}

	ret = AddHeader(name, value);

	return ret;
}

int HttpClient::SetTimeout(unsigned long milliSecond)
{
	mTimeoutMS = milliSecond;

	return 0;
}

int HttpClient::SyncGet()
{
	int ret;
	CURLcode curle;
	std::shared_ptr<CURL> curlHandlePtr;
	std::shared_ptr<struct curl_slist> curlHeadersPtr;

	ret = MakeCurl(curlHandlePtr, curlHeadersPtr);
	if(ret < 0) {
		return ret;
	}

	mRespStatus = -1;
	mRespReason.clear();
	mRespHeaders.clear();
	mRespBody = std::stringstream();

	curle = curl_easy_perform(curlHandlePtr.get());
	CHECK_ERROR(curle);

	curle = curl_easy_getinfo(curlHandlePtr.get(), CURLINFO_RESPONSE_CODE, &mRespStatus);
	CHECK_ERROR(curle);

	return 0;
}

int HttpClient::SyncPost(const int8_t* body, int size)
{
	int ret;
	CURLcode curle;
	std::shared_ptr<CURL> curlHandlePtr;
	std::shared_ptr<struct curl_slist> curlHeadersPtr;

	ret = MakeCurl(curlHandlePtr, curlHeadersPtr);
	if(ret < 0) {
		return ret;
	}

	mRespStatus = -1;
	mRespReason.clear();
	mRespHeaders.clear();
	mRespBody = std::stringstream();

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_POSTFIELDS, body);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_POSTFIELDSIZE, size);
	CHECK_ERROR(curle);

	curle = curl_easy_perform(curlHandlePtr.get());
	CHECK_ERROR(curle);

	curle = curl_easy_getinfo(curlHandlePtr.get(), CURLINFO_RESPONSE_CODE, &mRespStatus);
	CHECK_ERROR(curle);

	return 0;
}

int HttpClient::SyncPost(const std::string& body)
{
	int ret = 0;

	ret = SyncPost(reinterpret_cast<const int8_t*>(body.data()), body.size());

	return ret;
}

int HttpClient::GetResponseStatus() const
{
	return mRespStatus;
}

int HttpClient::GetResponseHeaders(HeaderMap& headers) const
{
	headers.clear();
	if((200 <= mRespStatus && mRespStatus < 300) == false) {
		return ErrCode::NetFailed;
	}

	headers = mRespHeaders;

	return mRespHeaders.size();
}

int HttpClient::GetResponseHeader(const std::string& name, HeaderValue& value) const
{
	value.clear();
	if((200 <= mRespStatus && mRespStatus < 300) == false) {
		return ErrCode::NetFailed;
	}
	auto found = mRespHeaders.find(name);
	if(found == mRespHeaders.end()) {
		return ErrCode::NotFound;
	}

	value = found->second;
	int size = found->second.size();

	return size;
}

int HttpClient::GetResponseBody(std::shared_ptr<int8_t>& body)
{
	body.reset();
	if((200 <= mRespStatus && mRespStatus < 300) == false) {
		return ErrCode::NetFailed;
	}

	mRespBody.seekg(0, mRespBody.end);
	int size = mRespBody.tellg();
	mRespBody.seekg(0, mRespBody.beg);

	body = std::shared_ptr<int8_t>(new int8_t[size], std::default_delete<int8_t[]>());
	mRespBody.read(reinterpret_cast<char*>(body.get()), size);

	return size;
}

int HttpClient::GetResponseBody(std::string& body)
{
	body.clear();
	if((200 <= mRespStatus && mRespStatus < 300) == false) {
		return ErrCode::NetFailed;
	}

	body = mRespBody.str();

	mRespBody.seekg(0, mRespBody.end);
	int size = mRespBody.tellg();
	mRespBody.seekg(0, mRespBody.beg);

	return size;
}

/* =========================================== */
/* === class protected function implement  === */
/* =========================================== */


/* =========================================== */
/* === class private function implement  ===== */
/* =========================================== */
std::string trim(const std::string &s)
{
   auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
   return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

size_t HttpClient::CurlHeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
	HttpClient* httpClient = static_cast<HttpClient*>(userdata);
	std::string header(buffer);
	size_t length = size * nitems;

	auto pos = header.find(':');
	if(pos != std::string::npos) {
		std::string name = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		name = trim(name);
		value = trim(value);

		httpClient->AddHeader(httpClient->mRespHeaders, name, value);
	}

	return length;
}

size_t HttpClient::CurlWriteCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
	HttpClient* httpClient = static_cast<HttpClient*>(userdata);
	size_t length = size * nitems;

	httpClient->mRespBody.write(buffer, length);

	return length;
}

size_t HttpClient::CurlReadCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{

	return -1;
}

int HttpClient::MakeCurl(std::shared_ptr<void>& curlHandlePtr, std::shared_ptr<struct curl_slist>& curlHeadersPtr) const
{
	CURLcode curle;

	curlHandlePtr.reset();
	curlHeadersPtr.reset();

	CURL*curlHandle = curl_easy_init();
	if(curlHandle == nullptr) {
		return (ErrCode::CurlBaseCode);
	}
	auto curlHandleDeleter = [](CURL* ptr) -> void {
		curl_easy_cleanup(ptr);
	};
	curlHandlePtr = std::shared_ptr<CURL>(curlHandle, curlHandleDeleter);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_NOSIGNAL, true);
	CHECK_ERROR(curle);

	if(mUrl.empty() == true) {
		return ErrCode::UrlNotExists;
	}
	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_URL, mUrl.c_str());
	CHECK_ERROR(curle);

	struct curl_slist* curlHeaders = nullptr;
	bool hasAcceptEncoding = false;
	for (auto& [key, val]: mReqHeaders) {
		auto& name = key;
		for(auto& value: val) {
			std::string header = (name + ": " + value);
			curlHeaders = curl_slist_append(curlHeaders, header.c_str());
		}

		if(key == "Accept-Encoding") {
			hasAcceptEncoding = true;
		}
	}
	if(hasAcceptEncoding == false) {
		curlHeaders = curl_slist_append(curlHeaders, "Accept-Encoding: identity");
	}
	auto curlHeadersDeleter = [](struct curl_slist* ptr) -> void {
		curl_slist_free_all(ptr);
	};
	curlHeadersPtr = std::shared_ptr<struct curl_slist>(curlHeaders, curlHeadersDeleter);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_HTTPHEADER, curlHeadersPtr.get());
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_TIMEOUT_MS, mTimeoutMS);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_HEADERFUNCTION, CurlHeaderCallback);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_HEADERDATA, this);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_WRITEFUNCTION, CurlWriteCallback);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_WRITEDATA, this);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_READFUNCTION, CurlReadCallback);
	CHECK_ERROR(curle);

	curle = curl_easy_setopt(curlHandlePtr.get(), CURLOPT_READDATA, this);
	CHECK_ERROR(curle);

	return 0;
}

int HttpClient::AddHeader(HeaderMap& headers, const std::string& name, const std::string& value) const
{
	if(name.empty() == true) {
		return ErrCode::NullArgument;
	}

	auto found = headers.find(name);
	if(found == headers.end()) {
		headers[name] = HeaderValue();
		found = headers.find(name);
	}

	found->second.push_back(value);

	return 0;
}

} // namespace elastos
