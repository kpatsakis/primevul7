    Jp2Image::Jp2Image(BasicIo::UniquePtr io, bool create)
            : Image(ImageType::jp2, mdExif | mdIptc | mdXmp, std::move(io))
    {
        if (create)
        {
            if (io_->open() == 0)
            {
#ifdef DEBUG
                std::cerr << "Exiv2::Jp2Image:: Creating JPEG2000 image to memory" << std::endl;
#endif
                IoCloser closer(*io_);
                if (io_->write(Jp2Blank, sizeof(Jp2Blank)) != sizeof(Jp2Blank))
                {
#ifdef DEBUG
                    std::cerr << "Exiv2::Jp2Image:: Failed to create JPEG2000 image on memory" << std::endl;
#endif
                }
            }
        }
    } // Jp2Image::Jp2Image