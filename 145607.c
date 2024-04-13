    int sonyCsSelector(uint16_t /*tag*/, const byte* /*pData*/, uint32_t /*size*/, TiffComponent* const pRoot)
    {
        std::string model = getExifModel(pRoot);
        if (model.empty()) return -1;
        int idx = 0;
        if (   model.find("DSLR-A330") != std::string::npos
            || model.find("DSLR-A380") != std::string::npos) {
            idx = 1;
        }
        return idx;
    }