    int DateValue::read(const std::string& buf)
    {
        // Hard coded to read Iptc style dates
        if (buf.length() < 8) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedDateFormat) << "\n";
#endif
            return 1;
        }
        int scanned = sscanf(buf.c_str(), "%4d-%d-%d",
                             &date_.year, &date_.month, &date_.day);
        if (scanned != 3) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedDateFormat) << "\n";
#endif
            return 1;
        }
        return 0;
    }