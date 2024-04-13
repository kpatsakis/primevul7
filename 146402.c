    PgfImage::PgfImage(BasicIo::UniquePtr io, bool create)
            : Image(ImageType::pgf, mdExif | mdIptc| mdXmp | mdComment, std::move(io))
            , bSwap_(isBigEndianPlatform())
    {
        if (create)
        {
            if (io_->open() == 0)
            {
#ifdef DEBUG
                std::cerr << "Exiv2::PgfImage:: Creating PGF image to memory\n";
#endif
                IoCloser closer(*io_);
                if (io_->write(pgfBlank, sizeof(pgfBlank)) != sizeof(pgfBlank))
                {
#ifdef DEBUG
                    std::cerr << "Exiv2::PgfImage:: Failed to create PGF image on memory\n";
#endif
                }
            }
        }
    } // PgfImage::PgfImage