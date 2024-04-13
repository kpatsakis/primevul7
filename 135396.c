MultiPartInputFile::Data::getPart(int partNumber)
{
    if (partNumber < 0 || partNumber >= (int) parts.size())
        throw IEX_NAMESPACE::ArgExc ("Part number is not in valid range.");
    return parts[partNumber];
}