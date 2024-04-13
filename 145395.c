    void Converter::cnvExifComment(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        const CommentValue* cv = dynamic_cast<const CommentValue*>(&pos->value());
        if (cv == 0) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        // Todo: Convert to UTF-8 if necessary
        (*xmpData_)[to] = cv->comment();
        if (erase_) exifData_->erase(pos);
    }