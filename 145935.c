    std::string getExifModel(Exiv2::Internal::TiffComponent* const pRoot)
    {
        Exiv2::Internal::TiffFinder finder(0x0110, Exiv2::Internal::ifd0Id); // Exif.Image.Model
        pRoot->accept(finder);
        Exiv2::Internal::TiffEntryBase* te = dynamic_cast<Exiv2::Internal::TiffEntryBase*>(finder.result());
        if (!te || !te->pValue() || te->pValue()->count() == 0) return std::string();
        return te->pValue()->toString();
    }