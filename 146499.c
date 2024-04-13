    int TimeValue::read(const std::string& buf)
    {
        // Hard coded to read H:M:S or Iptc style times
        int rc = 1;
        if (buf.length() < 9) {
            // Try to read (non-standard) H:M:S format
            rc = scanTime3(buf.c_str(), "%d:%d:%d");
        }
        else {
            rc = scanTime6(buf.c_str(), "%d:%d:%d%1c%d:%d");
        }
        if (rc) {
            rc = 1;
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedTimeFormat) << "\n";
#endif
        }
        return rc;
    }