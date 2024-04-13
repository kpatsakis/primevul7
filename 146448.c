    std::ostream& DateValue::write(std::ostream& os) const
    {
        std::ios::fmtflags f( os.flags() );
        os << date_.year << '-' << std::right
           << std::setw(2) << std::setfill('0') << date_.month << '-'
           << std::setw(2) << std::setfill('0') << date_.day;
        os.flags(f);
        return os;
    }