    int Print::printTag(const Exiv2::ExifData& exifData, EasyAccessFct easyAccessFct, const std::string& label) const
    {
        int rc = 0;
        if (!label.empty()) {
            printLabel(label);
        }
        auto md = easyAccessFct(exifData);
        if (md != exifData.end()) {
            md->write(std::cout, &exifData);
            rc = 1;
        }
        if (!label.empty())
            std::cout << std::endl;
        return rc;
    }