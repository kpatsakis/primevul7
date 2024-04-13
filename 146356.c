    int Adjust::adjustDateTime(Exiv2::ExifData& exifData, const std::string& key, const std::string& path) const
    {
        Exiv2::ExifKey ek(key);
        auto md = exifData.findKey(ek);
        if (md == exifData.end()) {
            // Key not found. That's ok, we do nothing.
            return 0;
        }
        std::string timeStr = md->toString();
        if (timeStr == "" || timeStr[0] == ' ') {
            std::cerr << path << ": " << _("Timestamp of metadatum with key") << " `" << ek << "' " << _("not set\n");
            return 1;
        }
        if (Params::instance().verbose_) {
            bool comma = false;
            std::cout << _("Adjusting") << " `" << ek << "' " << _("by");
            if (yearAdjustment_ != 0) {
                std::cout << (yearAdjustment_ < 0 ? " " : " +") << yearAdjustment_ << " ";
                if (yearAdjustment_ < -1 || yearAdjustment_ > 1) {
                    std::cout << _("years");
                } else {
                    std::cout << _("year");
                }
                comma = true;
            }
            if (monthAdjustment_ != 0) {
                if (comma)
                    std::cout << ",";
                std::cout << (monthAdjustment_ < 0 ? " " : " +") << monthAdjustment_ << " ";
                if (monthAdjustment_ < -1 || monthAdjustment_ > 1) {
                    std::cout << _("months");
                } else {
                    std::cout << _("month");
                }
                comma = true;
            }
            if (dayAdjustment_ != 0) {
                if (comma)
                    std::cout << ",";
                std::cout << (dayAdjustment_ < 0 ? " " : " +") << dayAdjustment_ << " ";
                if (dayAdjustment_ < -1 || dayAdjustment_ > 1) {
                    std::cout << _("days");
                } else {
                    std::cout << _("day");
                }
                comma = true;
            }
            if (adjustment_ != 0) {
                if (comma)
                    std::cout << ",";
                std::cout << " " << adjustment_ << _("s");
            }
        }
        struct tm tm;
        if (str2Tm(timeStr, &tm) != 0) {
            if (Params::instance().verbose_)
                std::cout << std::endl;
            std::cerr << path << ": " << _("Failed to parse timestamp") << " `" << timeStr << "'\n";
            return 1;
        }
        const long monOverflow = (tm.tm_mon + monthAdjustment_) / 12;
        tm.tm_mon = (tm.tm_mon + monthAdjustment_) % 12;
        tm.tm_year += yearAdjustment_ + monOverflow;
        // Let's not create files with non-4-digit years, we can't read them.
        if (tm.tm_year > 9999 - 1900 || tm.tm_year < 1000 - 1900) {
            if (Params::instance().verbose_)
                std::cout << std::endl;
            std::cerr << path << ": " << _("Can't adjust timestamp by") << " " << yearAdjustment_ + monOverflow << " "
                      << _("years") << "\n";
            return 1;
        }
        time_t time = mktime(&tm);
        time += adjustment_ + dayAdjustment_ * 86400;
        timeStr = time2Str(time);
        if (Params::instance().verbose_) {
            std::cout << " " << _("to") << " " << timeStr << std::endl;
        }
        md->setValue(timeStr);
        return 0;
    }