    TiffType toTiffType(TypeId typeId)
    {
        if (static_cast<uint32_t>(typeId) > 0xffff) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << "'" << TypeInfo::typeName(typeId)
                      << "' is not a valid Exif (TIFF) type; using type '"
                      << TypeInfo::typeName(undefined) << "'.\n";
#endif
            return undefined;
        }
        return static_cast<uint16_t>(typeId);
    }