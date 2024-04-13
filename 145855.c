    size_t curlWriter(char* data, size_t size, size_t nmemb,
                      std::string* writerData)
    {
        if (writerData == nullptr) return 0;
        writerData->append(data, size*nmemb);
        return size * nmemb;
    }