    int metacopy(const std::string& source, const std::string& tgt, Exiv2::ImageType targetType, bool preserve)
    {
#ifdef DEBUG
        std::cerr << "actions.cpp::metacopy"
                  << " source = " << source << " target = " << tgt << std::endl;
#endif

        // read the source metadata
        int rc = -1;
        if (!Exiv2::fileExists(source, true)) {
            std::cerr << source << ": " << _("Failed to open the file\n");
            return rc;
        }

        bool bStdin = source == "-";
        bool bStdout = tgt == "-";

        Exiv2::DataBuf stdIn;
        if (bStdin)
            Params::instance().getStdin(stdIn);
        Exiv2::BasicIo::UniquePtr ioStdin = Exiv2::BasicIo::UniquePtr(new Exiv2::MemIo(stdIn.pData_, stdIn.size_));

        Exiv2::Image::UniquePtr sourceImage =
            bStdin ? Exiv2::ImageFactory::open(std::move(ioStdin)) : Exiv2::ImageFactory::open(source);
        sourceImage->readMetadata();

        // Apply any modification commands to the source image on-the-fly
        Action::Modify::applyCommands(sourceImage.get());

        // Open or create the target file
        std::string target(bStdout ? temporaryPath() : tgt);

        Exiv2::Image::UniquePtr targetImage;
        if (Exiv2::fileExists(target)) {
            targetImage = Exiv2::ImageFactory::open(target);
            targetImage->readMetadata();
        } else {
            targetImage = Exiv2::ImageFactory::create(targetType, target);
            assert(targetImage.get() != 0);
        }

        // Copy each type of metadata
        if (Params::instance().target_ & Params::ctExif && !sourceImage->exifData().empty()) {
            if (Params::instance().verbose_) {
                std::cout << _("Writing Exif data from") << " " << source << " " << _("to") << " " << target
                          << std::endl;
            }
            if (preserve) {
                for (auto i = sourceImage->exifData().begin(); i != sourceImage->exifData().end(); ++i) {
                    targetImage->exifData()[i->key()] = i->value();
                }
            } else {
                targetImage->setExifData(sourceImage->exifData());
            }
        }
        if (Params::instance().target_ & Params::ctIptc && !sourceImage->iptcData().empty()) {
            if (Params::instance().verbose_) {
                std::cout << _("Writing IPTC data from") << " " << source << " " << _("to") << " " << target
                          << std::endl;
            }
            if (preserve) {
                for (auto i = sourceImage->iptcData().begin(); i != sourceImage->iptcData().end(); ++i) {
                    targetImage->iptcData()[i->key()] = i->value();
                }
            } else {
                targetImage->setIptcData(sourceImage->iptcData());
            }
        }
        if (Params::instance().target_ & (Params::ctXmp | Params::ctXmpRaw) && !sourceImage->xmpData().empty()) {
            if (Params::instance().verbose_) {
                std::cout << _("Writing XMP data from") << " " << source << " " << _("to") << " " << target
                          << std::endl;
            }

            // #1148 use Raw XMP packet if there are no XMP modification commands
            int tRawSidecar = Params::ctXmpSidecar | Params::ctXmpRaw;  // option -eXX
            // printTarget("in metacopy",Params::instance().target_,true);
            if (Params::instance().modifyCmds_.size() == 0 &&
                (Params::instance().target_ & tRawSidecar) == tRawSidecar) {
                // std::cout << "short cut" << std::endl;
                // http://www.cplusplus.com/doc/tutorial/files/
                std::ofstream os;
                os.open(target.c_str());
                sourceImage->printStructure(os, Exiv2::kpsXMP);
                os.close();
                rc = 0;
            } else if (preserve) {
                for (auto i = sourceImage->xmpData().begin(); i != sourceImage->xmpData().end(); ++i) {
                    targetImage->xmpData()[i->key()] = i->value();
                }
            } else {
                // std::cout << "long cut" << std::endl;
                targetImage->setXmpData(sourceImage->xmpData());
            }
        }
        if (Params::instance().target_ & Params::ctComment && !sourceImage->comment().empty()) {
            if (Params::instance().verbose_) {
                std::cout << _("Writing JPEG comment from") << " " << source << " " << _("to") << " " << tgt
                          << std::endl;
            }
            targetImage->setComment(sourceImage->comment());
        }
        if (rc < 0)
            try {
                targetImage->writeMetadata();
                rc = 0;
            } catch (const Exiv2::AnyError& e) {
                std::cerr << tgt << ": " << _("Could not write metadata to file") << ": " << e << "\n";
                rc = 1;
            }

        // if we used a temporary target, copy it to stdout
        if (rc == 0 && bStdout) {
            FILE* f = ::fopen(target.c_str(), "rb");
            _setmode(_fileno(stdout), O_BINARY);

            if (f) {
                char buffer[8 * 1024];
                size_t n = 1;
                while (!feof(f) && n > 0) {
                    n = fread(buffer, 1, sizeof buffer, f);
                    fwrite(buffer, 1, n, stdout);
                }
                fclose(f);
            }
        }

        // delete temporary target
        if (bStdout)
            std::remove(target.c_str());

        return rc;
    }