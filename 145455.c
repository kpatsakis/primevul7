    int Modify::applyCommands(Exiv2::Image* pImage)
    {
        if (!Params::instance().jpegComment_.empty()) {
            if (Params::instance().verbose_) {
                std::cout << _("Setting JPEG comment") << " '" << Params::instance().jpegComment_ << "'" << std::endl;
            }
            pImage->setComment(Params::instance().jpegComment_);
        }

        // loop through command table and apply each command
        ModifyCmds& modifyCmds = Params::instance().modifyCmds_;
        int rc = 0;
        int ret = 0;
        for (auto i = modifyCmds.cbegin(); i != modifyCmds.cend(); ++i) {
            switch (i->cmdId_) {
                case add:
                    ret = addMetadatum(pImage, *i);
                    if (rc == 0)
                        rc = ret;
                    break;
                case set:
                    ret = setMetadatum(pImage, *i);
                    if (rc == 0)
                        rc = ret;
                    break;
                case del:
                    delMetadatum(pImage, *i);
                    break;
                case reg:
                    regNamespace(*i);
                    break;
                case invalidCmdId:
                    assert(invalidCmdId == i->cmdId_);
                    break;
            }
        }
        return rc;
    }