#ifndef AUTHOR_INFO_H
#define AUTHOR_INFO_H

#include <atomic>
#include <iostream>

namespace AuthorInfo {
    class LibraryInitializer {
    public:
        LibraryInitializer();
    private:
        // void updateVersion() const;
        const char* mVersion;
        // static size_t writeCallback(void *contents, size_t size, size_t nmemb, std::string *response);
    };

    // void forceLinkAuthorInfo();
}

#endif