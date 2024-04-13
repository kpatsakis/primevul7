    void Converter::cnvExifFlash(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end() || pos->count() == 0) return;
        if (!prepareXmpTarget(to)) return;
        int value = pos->toLong();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        (*xmpData_)["Xmp.exif.Flash/exif:Fired"] = static_cast<bool>(value & 1);
        (*xmpData_)["Xmp.exif.Flash/exif:Return"] = (value >> 1) & 3;
        (*xmpData_)["Xmp.exif.Flash/exif:Mode"] = (value >> 3) & 3;
        (*xmpData_)["Xmp.exif.Flash/exif:Function"] = static_cast<bool>((value >> 5) & 1);
        (*xmpData_)["Xmp.exif.Flash/exif:RedEyeMode"] = static_cast<bool>((value >> 6) & 1);

        if (erase_) exifData_->erase(pos);
    }