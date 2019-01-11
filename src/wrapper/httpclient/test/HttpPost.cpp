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
		
	ret = httpClient.Url("https://hw-did-api-test.elastos.org/api/1/createTx");
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.SetTimeout(10000);
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.SetHeader("Content-Type", "application/json");
	std::cout << "return code: " << ret << std::endl;

	std::string body;
	body.append("{");
	body.append("\"inputs\":[\"EPzxJrHefvE7TCWmEGQ4rcFgxGeGBZFSHw\"],");
	body.append("\"outputs\":[{");
	body.append("\"addr\":\"EPzxJrHefvE7TCWmEGQ4rcFgxGeGBZFSHw\",");
	body.append("\"amt\":0");
	body.append("}]");
	body.append("}");
	std::cout << "body: " << body << std::endl;

	ret = httpClient.SyncPost(body);
	std::cout << "return code2: " << ret << std::endl;

	ret = httpClient.SyncPost(reinterpret_cast<const int8_t*>(body.c_str()), body.length());
	std::cout << "return code: " << ret << std::endl;

	ret = httpClient.GetResponseStatus();
	std::cout << "return resp status: " << ret << std::endl;

	ret = httpClient.GetResponseBody(body);
	std::cout << "response size: " << ret << std::endl;
	std::cout << "response text: " << body << std::endl;

	elastos::HttpClient::HeaderValue header;
	ret = httpClient.GetResponseHeader("Content-Length", header);
	std::cout << "return code: " << ret << std::endl;

	return 0;
}
