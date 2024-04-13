    void TiffHeaderBase::print(std::ostream& os, const std::string& prefix) const
    {
        std::ios::fmtflags f( os.flags() );
        os << prefix
           << _("TIFF header, offset") << " = 0x"
           << std::setw(8) << std::setfill('0') << std::hex << std::right
           << offset_;

        switch (byteOrder_) {
        case littleEndian:     os << ", " << _("little endian encoded"); break;
        case bigEndian:        os << ", " << _("big endian encoded");    break;
        case invalidByteOrder: break;
        }
        os << "\n";
        os.flags(f);
    } // TiffHeaderBase::print