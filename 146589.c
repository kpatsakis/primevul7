    int Print::printTag(const Exiv2::ExifData& exifData, const std::string& key, const std::string& label) const
    {
        int rc = 0;
        if (!label.empty()) {
            printLabel(label);
        }
        Exiv2::ExifKey ek(key);
        auto md = exifData.findKey(ek);
        if (md != exifData.end()) {
            md->write(std::cout, &exifData);
            rc = 1;
        }
        if (!label.empty())
            std::cout << std::endl;
        return rc;
    }