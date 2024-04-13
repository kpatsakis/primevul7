    int Erase::eraseComment(Exiv2::Image* image) const
    {
        if (Params::instance().verbose_ && image->comment().size() > 0) {
            std::cout << _("Erasing JPEG comment from the file") << std::endl;
        }
        image->clearComment();
        return 0;
    }