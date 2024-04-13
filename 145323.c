    std::ostream& Xmpdatum::write(std::ostream& os, const ExifData*) const
    {
        return XmpProperties::printProperty(os, key(), value());
    }