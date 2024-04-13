    void Converter::cnvXmpGPSCoord(const char* from, const char* to)
    {
        Exiv2::XmpData::iterator pos = xmpData_->findKey(XmpKey(from));
        if (pos == xmpData_->end()) return;
        if (!prepareExifTarget(to)) return;
        std::string value = pos->toString();
        if (!pos->value().ok()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }
        if (value.empty()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << from << " is empty\n";
#endif
            return;
        }

        double deg = 0.0;
        double min = 0.0;
        double sec = 0.0;
        char ref  = value[value.length() - 1];
        char sep1 = '\0';
        char sep2 = '\0';

        value.erase(value.length() - 1);

        std::istringstream in(value);

        in >> deg >> sep1 >> min >> sep2;

        if (sep2 == ',') {
            in >> sec;
        }
        else {
            sec = (min - static_cast<int>(min)) * 60.0;
            min = static_cast<double>(static_cast<int>(min));
            sep2 = ',';
        }

        if (   in.bad() || !(ref == 'N' || ref == 'S' || ref == 'E' || ref == 'W')
            || sep1 != ',' || sep2 != ',' || !in.eof()) {
#ifndef SUPPRESS_WARNINGS
            EXV_WARNING << "Failed to convert " << from << " to " << to << "\n";
#endif
            return;
        }

        Rational rdeg = floatToRationalCast(static_cast<float>(deg));
        Rational rmin = floatToRationalCast(static_cast<float>(min));
        Rational rsec = floatToRationalCast(static_cast<float>(sec));

        std::ostringstream array;
        array << rdeg << " " << rmin << " " << rsec;
        (*exifData_)[to] = array.str();

        prepareExifTarget((std::string(to) + "Ref").c_str(), true);
        char ref_str[2] = {ref, 0};
        (*exifData_)[std::string(to) + "Ref"] = ref_str;

        if (erase_) xmpData_->erase(pos);
    }