    int Erase::eraseIptcData(Exiv2::Image* image) const
    {
        if (Params::instance().verbose_ && image->iptcData().count() > 0) {
            std::cout << _("Erasing IPTC data from the file") << std::endl;
        }
        image->clearIptcData();
        return 0;
    }