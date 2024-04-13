    int printStructure(std::ostream& out, Exiv2::PrintStructureOption option, const std::string& path)
    {
        if (!Exiv2::fileExists(path, true)) {
            std::cerr << path << ": " << _("Failed to open the file\n");
            return -1;
        }
        Exiv2::Image::UniquePtr image = Exiv2::ImageFactory::open(path);
        image->printStructure(out, option);
        return 0;
    }