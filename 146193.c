    int Erase::eraseIccProfile(Exiv2::Image* image) const
    {
        if (Params::instance().verbose_ && image->iccProfileDefined()) {
            std::cout << _("Erasing ICC Profile data from the file") << std::endl;
        }
        image->clearIccProfile();
        return 0;
    }