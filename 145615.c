    int DateValue::read(const byte* buf, size_t len, ByteOrder /*byteOrder*/)
    {
        // Hard coded to read Iptc style dates
        if (len != 8) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedDateFormat) << "\n";
#endif
            return 1;
        }
        // Make the buffer a 0 terminated C-string for sscanf
        char b[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        std::memcpy(b, reinterpret_cast<const char*>(buf), 8);
        int scanned = sscanf(b, "%4d%2d%2d",
                             &date_.year, &date_.month, &date_.day);
        if (scanned != 3) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << Error(kerUnsupportedDateFormat) << "\n";
#endif
            return 1;
        }
        return 0;
    }