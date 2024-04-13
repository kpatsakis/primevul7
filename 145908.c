    bool isXmpType(BasicIo& iIo, bool advance)
    {
        /*
          Check if the file starts with an optional XML declaration followed by
          either an XMP header (<?xpacket ... ?>) or an <x:xmpmeta> element.

          In addition, in order for empty XmpSidecar objects as created by
          Exiv2 to pass the test, just an XML header is also considered ok.
         */
        const int32_t len = 80;
        byte buf[len];
        iIo.read(buf, xmlHdrCnt + 1);
        if (   iIo.eof()
            && 0 == strncmp(reinterpret_cast<const char*>(buf), xmlHeader, xmlHdrCnt)) {
            return true;
        }
        if (iIo.error() || iIo.eof()) {
            return false;
        }
        iIo.read(buf + xmlHdrCnt + 1, len - xmlHdrCnt - 1);
        if (iIo.error() || iIo.eof()) {
            return false;
        }
        // Skip leading BOM
        int32_t start = 0;
        if (0 == strncmp(reinterpret_cast<const char*>(buf), "\xef\xbb\xbf", 3)) {
            start = 3;
        }
        bool rc = false;
        std::string head(reinterpret_cast<const char*>(buf + start), len - start);
        if (head.substr(0, 5)  == "<?xml") {
            // Forward to the next tag
            for (unsigned i = 5; i < head.size(); ++i) {
                if (head[i] == '<') {
                    head = head.substr(i);
                    break;
                }
            }
        }
        if (   head.size() > 9
            && (   head.substr(0, 9)  == "<?xpacket"
                || head.substr(0, 10) == "<x:xmpmeta")) {
            rc = true;
        }
        if (!advance || !rc) {
            iIo.seek(-(len - start), BasicIo::cur); // Swallow the BOM
        }
        return rc;

    }