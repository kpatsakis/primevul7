    void Converter::cnvExifDate(const char* from, const char* to)
    {
        Exiv2::ExifData::iterator pos = exifData_->findKey(ExifKey(from));
        if (pos == exifData_->end()) return;
        if (!prepareXmpTarget(to)) return;
        int year, month, day, hour, min, sec;
        std::string subsec;
        char buf[30];

        if (std::string(from) != "Exif.GPSInfo.GPSTimeStamp") {
            std::string value = pos->toString();
            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            if (sscanf(value.c_str(), "%d:%d:%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec) != 6) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to
                            << ", unable to parse '" << value << "'\n";
#endif
                return;
            }
        }
        else { // "Exif.GPSInfo.GPSTimeStamp"

            bool ok = true;
            if (pos->count() != 3) ok = false;
            if (ok) {
                for (int i = 0; i < 3; ++i) {
                    if (pos->toRational(i).second == 0) {
                        ok = false;
                        break;
                    }
                }
            }
            if (!ok) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }

            double dhour = pos->toFloat(0);
            double dmin = pos->toFloat(1);
            // Hack: Need Value::toDouble
            Rational r = pos->toRational(2);
            double dsec = static_cast<double>(r.first)/r.second;

            if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }

            dsec = dhour * 3600.0 + dmin * 60.0 + dsec;

            hour = static_cast<int>(dsec / 3600.0);
            dsec -= hour * 3600;
            min = static_cast<int>(dsec / 60.0);
            dsec -= min * 60;
            sec = static_cast<int>(dsec);
            dsec -= sec;

            snprintf(buf, sizeof(buf), "%.9f", dsec);
            buf[sizeof(buf) - 1] = 0;
            buf[1] = '.'; // some locales use ','
            subsec = buf + 1;

            Exiv2::ExifData::iterator datePos = exifData_->findKey(ExifKey("Exif.GPSInfo.GPSDateStamp"));
            if (datePos == exifData_->end()) {
                datePos = exifData_->findKey(ExifKey("Exif.Photo.DateTimeOriginal"));
            }
            if (datePos == exifData_->end()) {
                datePos = exifData_->findKey(ExifKey("Exif.Photo.DateTimeDigitized"));
            }
            if (datePos == exifData_->end()) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
                return;
            }
            std::string value = datePos->toString();
            if (sscanf(value.c_str(), "%d:%d:%d", &year, &month, &day) != 3) {
#ifndef SUPPRESS_WARNINGS
                EXV_WARNING << "Failed to convert " << from << " to " << to
                            << ", unable to parse '" << value << "'\n";
#endif
                return;
            }
        }

        const char* subsecTag = 0;
        if (std::string(from) == "Exif.Image.DateTime") {
            subsecTag = "Exif.Photo.SubSecTime";
        }
        else if (std::string(from) == "Exif.Photo.DateTimeOriginal") {
            subsecTag = "Exif.Photo.SubSecTimeOriginal";
        }
        else if (std::string(from) == "Exif.Photo.DateTimeDigitized") {
            subsecTag = "Exif.Photo.SubSecTimeDigitized";
        }

        if (subsecTag) {
            ExifData::iterator subsec_pos = exifData_->findKey(ExifKey(subsecTag));
            if (   subsec_pos != exifData_->end()
                && subsec_pos->typeId() == asciiString) {
                std::string ss = subsec_pos->toString();
                if (!ss.empty()) {
                    bool ok = false;
                    stringTo<long>(ss, ok);
                    if (ok) subsec = std::string(".") + ss;
                }
            }
            if (erase_) exifData_->erase(subsec_pos);
        }

        if (subsec.size() > 10) subsec = subsec.substr(0, 10);
        snprintf(buf, sizeof(buf), "%4d-%02d-%02dT%02d:%02d:%02d%s",
                 year, month, day, hour, min, sec, subsec.c_str());
        buf[sizeof(buf) - 1] = 0;

        (*xmpData_)[to] = buf;
        if (erase_) exifData_->erase(pos);
    }