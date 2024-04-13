    int setModeAndPrintStructure(Exiv2::PrintStructureOption option, const std::string& path)
    {
        _setmode(_fileno(stdout), O_BINARY);
        return printStructure(std::cout, option, path);
    }