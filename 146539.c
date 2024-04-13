    void hexdump(std::ostream& os, const byte* buf, long len, long offset)
    {
        const std::string::size_type pos = 8 + 16 * 3 + 2;
        const std::string align(pos, ' ');
        std::ios::fmtflags f( os.flags() );

        long i = 0;
        while (i < len) {
            os << "  "
               << std::setw(4) << std::setfill('0') << std::hex
               << i + offset << "  ";
            std::ostringstream ss;
            do {
                byte c = buf[i];
                os << std::setw(2) << std::setfill('0') << std::right
                   << std::hex << (int)c << " ";
                ss << ((int)c >= 31 && (int)c < 127 ? char(buf[i]) : '.');
            } while (++i < len && i%16 != 0);
            std::string::size_type width = 9 + ((i-1)%16 + 1) * 3;
            os << (width > pos ? "" : align.substr(width)) << ss.str() << "\n";
        }
        os << std::dec << std::setfill(' ');
        os.flags(f);
    } // hexdump