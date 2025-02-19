    void Converter::cnvXmpFlash(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Fired"));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        unsigned short value = 0;

        if (pos != xmpData_->end() && pos->count() > 0) {
            int fired = pos->toLong();
            if (pos->value().ok())
                value |= fired & 1;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Fired" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Return"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int ret = pos->toLong();
            if (pos->value().ok())
                value |= (ret & 3) << 1;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Return" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Mode"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int mode = pos->toLong();
            if (pos->value().ok())
                value |= (mode & 3) << 3;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Mode" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:Function"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int function = pos->toLong();
            if (pos->value().ok())
                value |= (function & 1) << 5;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:Function" << " to " << to << "\n";
#endif
        }
        pos = xmpData_->findKey(XmpKey(std::string(from) + "/exif:RedEyeMode"));
        if (pos != xmpData_->end() && pos->count() > 0) {
            int red = pos->toLong();
            if (pos->value().ok())
                value |= (red & 1) << 6;
#ifndef SUPPRESS_WARNINGS
            else
                EXV_WARNING << "Failed to convert " << std::string(from) + "/exif:RedEyeMode" << " to " << to << "\n";
#endif
        }

        (*exifData_)[to] = value;
        if (erase_) xmpData_->erase(pos);
    }