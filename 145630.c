    std::ostream& Exifdatum::write(std::ostream& os, const ExifData* pMetadata) const
    {
        if (value().count() == 0) return os;
        PrintFct fct = printValue;
        const TagInfo* ti = Internal::tagInfo(tag(), static_cast<IfdId>(ifdId()));
        if (ti != 0) fct = ti->printFct_;
        return fct(os, value(), pMetadata);
    }