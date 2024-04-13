    int TimeValue::read(const byte* buf, size_t len, ByteOrder /*byteOrder*/)
    {
        // Make the buffer a 0 terminated C-string for scanTime[36]
        char b[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        std::memcpy(b, reinterpret_cast<const char*>(buf), (len < 12 ? len : 11));
        // Hard coded to read HHMMSS or Iptc style times
        int rc = 1;
        if (len == 6) {
            // Try to read (non-standard) HHMMSS format
            rc = scanTime3(b, "%2d%2d%2d");
        }
        if (len == 11) {
            rc = scanTime6(b, "%2d%2d%2d%1c%2d%2d");
        }
        if (rc) {
            rc = 1;
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedTimeFormat) << "\n";
#endif
        }
        return rc;
    }