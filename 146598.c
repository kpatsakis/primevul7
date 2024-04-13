    std::ostream& Iptcdatum::write(std::ostream& os, const ExifData*) const
    {
        return os << value();
    }