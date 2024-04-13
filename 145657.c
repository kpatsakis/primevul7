    bool Image::isPrintXMP(uint16_t type, Exiv2::PrintStructureOption option)
    {
        return type == 700 && option == kpsXMP;
    }