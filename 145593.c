    void Converter::cnvExifGPSCoord(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        if (pos->count() != 3) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        Exiv2::ExifData::iterator refPos = exifData_->findKey(ExifKey(std::string(from) + "Ref"));
        if (refPos == exifData_->end()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        double deg[3];
        for (int i = 0; i < 3; ++i) {
            const int32_t z = pos->toRational(i).first;
            const int32_t d = pos->toRational(i).second;
            if (d == 0) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            // Hack: Need Value::toDouble
            deg[i] = static_cast<double>(z)/d;
        }
        double min = deg[0] * 60.0 + deg[1] + deg[2] / 60.0;
        int ideg = static_cast<int>(min / 60.0);
        min -= ideg * 60;
        std::ostringstream oss;
        oss << ideg << ","
            << std::fixed << std::setprecision(7) << min
            << refPos->toString().c_str()[0];
        (*xmpData_)[to] = oss.str();

        if (erase_) exifData_->erase(pos);
        if (erase_) exifData_->erase(refPos);
    }