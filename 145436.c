    void Converter::cnvXmpDate(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
#ifdef EXV_HAVE_XMP_TOOLKIT
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        XMP_DateTime datetime;
        try {
            SXMPUtils::ConvertToDate(value, &datetime);
        }
#ifndef SUPPRESS_WARNINGS
        catch (const XMP_Error& e) {
            EXV_WARNING << "Failed to convert " << from << " to " << to << " (" << e.GetErrMsg() << ")\n";
            return;
        }
#else
        catch (const XMP_Error&) {
            return;
        }
#endif // SUPPRESS_WARNINGS
        char buf[30];
        if (std::string(to) != "Exif.GPSInfo.GPSTimeStamp") {

            SXMPUtils::ConvertToLocalTime(&datetime);

            snprintf(buf, sizeof(buf), "%4d:%02d:%02d %02d:%02d:%02d",
                     static_cast<int>(datetime.year),
                     static_cast<int>(datetime.month),
                     static_cast<int>(datetime.day),
                     static_cast<int>(datetime.hour),
                     static_cast<int>(datetime.minute),
                     static_cast<int>(datetime.second));
            buf[sizeof(buf) - 1] = 0;
            (*exifData_)[to] = buf;

            if (datetime.nanoSecond) {
                const char* subsecTag = 0;
                if (std::string(to) == "Exif.Image.DateTime") {
                    subsecTag = "Exif.Photo.SubSecTime";
                }
                else if (std::string(to) == "Exif.Photo.DateTimeOriginal") {
                    subsecTag = "Exif.Photo.SubSecTimeOriginal";
                }
                else if (std::string(to) == "Exif.Photo.DateTimeDigitized") {
                    subsecTag = "Exif.Photo.SubSecTimeDigitized";
                }
                if (subsecTag) {
                    prepareExifTarget(subsecTag, true);
                    (*exifData_)[subsecTag] = toString(datetime.nanoSecond);
                }
            }
        }
        else { // "Exif.GPSInfo.GPSTimeStamp"

            // Ignore the time zone, assuming the time is in UTC as it should be

            URational rhour(datetime.hour, 1);
            URational rmin(datetime.minute, 1);
            URational rsec(datetime.second, 1);
            if (datetime.nanoSecond != 0) {
                if (datetime.second != 0) {
                    // Add the seconds to rmin so that the ns fit into rsec
                    rmin.second = 60;
                    rmin.first *= 60;
                    rmin.first += datetime.second;
                }
                rsec.second = 1000000000;
                rsec.first = datetime.nanoSecond;
            }

            std::ostringstream array;
            array << rhour << " " << rmin << " " << rsec;
            (*exifData_)[to] = array.str();

            prepareExifTarget("Exif.GPSInfo.GPSDateStamp", true);
            snprintf(buf, sizeof(buf), "%4d:%02d:%02d",
                    static_cast<int>(datetime.year),
                    static_cast<int>(datetime.month),
                    static_cast<int>(datetime.day));
            buf[sizeof(buf) - 1] = 0;
            (*exifData_)["Exif.GPSInfo.GPSDateStamp"] = buf;
        }

        if (erase_) xmpData_->erase(pos);
#else
# ifndef SUPPRESS_WARNINGS
        EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
# endif
#endif // !EXV_HAVE_XMP_TOOLKIT
    }