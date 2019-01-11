#include <iostream>

#include <HttpClient.hpp>

int main( int argc, char **argv )
{
	int ret = 0;
	elastos::HttpClient httpClient;

	elastos::HttpClient::InitGlobal();

	std::cout << "error check test: "<< std::endl;
	ret = httpClient.Url("");
	std::cout << "return code: " << ret << std::endl;
	ret = httpClient.Url("haa");
	std::cout << "return code: " << ret << std::endl;
	std::cout << "=================="<< std::endl;
		
	ret = httpClient.Url("https://hw-did-api-test.elastos.org/api/1/currHeight");
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.SetTimeout(5000);
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.SetHeader("Accept-Encoding", "identity");
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.SyncGet();
	std::cout << "return code: " << ret << std::endl;

	std::shared_ptr<int8_t> body;
	ret = httpClient.GetResponseBody(body);
	std::cout << "return code: " << ret << std::endl;

	std::string bodyStr;
	ret = httpClient.GetResponseBody(bodyStr);
	std::cout << "return code: " << ret << std::endl;

	std::cout << "response size: " << ret << std::endl;
	std::cout << "response text: " << std::string(reinterpret_cast<char*>(body.get()), ret) << std::endl;

	elastos::HttpClient::HeaderMap headermap;
	ret = httpClient.GetResponseHeaders(headermap);
	std::cout << "return code: " << ret << std::endl;

	return 0;
}
