#include "AuthorInfo.h"
// #include <curl/curl.h>
namespace AuthorInfo {
    static std::atomic<bool> authorInfoPrinted(false);

    LibraryInitializer::LibraryInitializer() :
        mVersion("1.0") {
        if (!authorInfoPrinted.exchange(true)) {

            std::cout << "-----------------------------" << std::endl;
            std::cout << "Library: torosamy_huffman_compressor" << std::endl;
            std::cout << "Author: Torosamy" << std::endl;
            std::cout << "Website: www.torosamy.net" << std::endl;
            std::cout << "Version: "<<mVersion<<" - GPL-3.0 license" << std::endl;
            

            // if(fileReader["Settings"]["CheckVersion"]["Enable"].as<bool>()) updateVersion();
            // else std::cout<<"Warning: Version checking has been disabled"<<std::endl;

            std::cout << "If you have any questions, welcome to contact me" << std::endl;
            std::cout << "-----------------------------" << std::endl;
        }
    }
    // size_t LibraryInitializer::writeCallback(void *contents, size_t size, size_t nmemb, std::string *response) {
    //     size_t totalSize = size * nmemb;
    //     response->append((char*)contents, totalSize);
    //     return totalSize;
    // }
    // void LibraryInitializer::updateVersion() const {
    //     std::string readBuffer;
        
    //     curl_global_init(CURL_GLOBAL_DEFAULT);
    
    //     CURL *curl = curl_easy_init();
    //     if(!curl) {
    //         std::cerr << "curl_easy_init() failed" << std::endl;
    //         curl_global_cleanup();
    //     } 


    //     curl_easy_setopt(curl, CURLOPT_URL, "https://server.torosamy.net:11699/version/check/robomaster");
    //     curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, LibraryInitializer::writeCallback);
    //     curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    //     const CURLcode res = curl_easy_perform(curl);

    //     if(res == CURLE_OK) {
    //         std::cout << "NewestVersion: " << readBuffer << std::endl;
    //     } else {
    //         std::cerr << "Error: " << curl_easy_strerror(res) << std::endl;
    //     }

    //     curl_easy_cleanup(curl);
    // }
    static LibraryInitializer libraryInitializer;

}