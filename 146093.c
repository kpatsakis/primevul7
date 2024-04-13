    bool Print::printMetadatum(const Exiv2::Metadatum& md, const Exiv2::Image* pImage)
    {
        if (!grepTag(md.key()))
            return false;
        if (!keyTag(md.key()))
            return false;

        if (Params::instance().unknown_ && md.tagName().substr(0, 2) == "0x") {
            return false;
        }

        bool const manyFiles = Params::instance().files_.size() > 1;
        if (manyFiles) {
            std::cout << std::setfill(' ') << std::left << std::setw(20) << path_ << "  ";
        }

        bool first = true;
        if (Params::instance().printItems_ & Params::prTag) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << "0x" << std::setw(4) << std::setfill('0') << std::right << std::hex << md.tag();
        }
        if (Params::instance().printItems_ & Params::prSet) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << "set";
        }
        if (Params::instance().printItems_ & Params::prGroup) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::setw(12) << std::setfill(' ') << std::left << md.groupName();
        }
        if (Params::instance().printItems_ & Params::prKey) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::setfill(' ') << std::left << std::setw(44) << md.key();
        }
        if (Params::instance().printItems_ & Params::prName) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::setw(27) << std::setfill(' ') << std::left << md.tagName();
        }
        if (Params::instance().printItems_ & Params::prLabel) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::setw(30) << std::setfill(' ') << std::left << md.tagLabel();
        }
        if (Params::instance().printItems_ & Params::prType) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::setw(9) << std::setfill(' ') << std::left;
            const char* tn = md.typeName();
            if (tn) {
                std::cout << tn;
            } else {
                std::ostringstream os;
                os << "0x" << std::setw(4) << std::setfill('0') << std::hex << md.typeId();
                std::cout << os.str();
            }
        }
        if (Params::instance().printItems_ & Params::prCount) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::dec << std::setw(3) << std::setfill(' ') << std::right << md.count();
        }
        if (Params::instance().printItems_ & Params::prSize) {
            if (!first)
                std::cout << " ";
            first = false;
            std::cout << std::dec << std::setw(3) << std::setfill(' ') << std::right << md.size();
        }
        if (Params::instance().printItems_ & Params::prValue && md.size() > 0) {
            if (!first)
                std::cout << "  ";
            first = false;
            if (md.size() > 128 && Params::instance().binary_ &&
                (md.typeId() == Exiv2::undefined || md.typeId() == Exiv2::unsignedByte ||
                 md.typeId() == Exiv2::signedByte)) {
                std::cout << _("(Binary value suppressed)") << std::endl;
                return true;
            }
            bool done = false;
            if (0 == strcmp(md.key().c_str(), "Exif.Photo.UserComment")) {
                const Exiv2::CommentValue* pcv = dynamic_cast<const Exiv2::CommentValue*>(&md.value());
                if (pcv) {
                    Exiv2::CommentValue::CharsetId csId = pcv->charsetId();
                    if (csId != Exiv2::CommentValue::undefined) {
                        std::cout << "charset=\"" << Exiv2::CommentValue::CharsetInfo::name(csId) << "\" ";
                    }
                    std::cout << pcv->comment(Params::instance().charset_.c_str());
                    done = true;
                }
            }
            if (!done) {
                // #1114 - show negative values for SByte
                if (md.typeId() != Exiv2::signedByte) {
                    std::cout << std::dec << md.value();
                } else {
                    long value = md.value().toLong();
                    std::cout << std::dec << (value < 128 ? value : value - 256);
                }
            }
        }
        if (Params::instance().printItems_ & Params::prTrans) {
            if (!first)
                std::cout << "  ";
            first = false;
            if (Params::instance().binary_ &&
                (md.typeId() == Exiv2::undefined || md.typeId() == Exiv2::unsignedByte ||
                 md.typeId() == Exiv2::signedByte) &&
                md.size() > 128) {
                std::cout << _("(Binary value suppressed)") << std::endl;
                return true;
            }
            bool done = false;
            if (0 == strcmp(md.key().c_str(), "Exif.Photo.UserComment")) {
                const Exiv2::CommentValue* pcv = dynamic_cast<const Exiv2::CommentValue*>(&md.value());
                if (pcv) {
                    std::cout << pcv->comment(Params::instance().charset_.c_str());
                    done = true;
                }
            }
            if (!done)
                std::cout << std::dec << md.print(&pImage->exifData());
        }
        if (Params::instance().printItems_ & Params::prHex) {
            if (!first)
                std::cout << std::endl;
            first = false;
            if (Params::instance().binary_ &&
                (md.typeId() == Exiv2::undefined || md.typeId() == Exiv2::unsignedByte ||
                 md.typeId() == Exiv2::signedByte) &&
                md.size() > 128) {
                std::cout << _("(Binary value suppressed)") << std::endl;
                return true;
            }
            Exiv2::DataBuf buf(md.size());
            md.copy(buf.pData_, pImage->byteOrder());
            Exiv2::hexdump(std::cout, buf.pData_, (long)buf.size_);
        }
        std::cout << std::endl;
        return true;
    }