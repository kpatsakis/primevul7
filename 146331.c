    void eraseIfd(Exiv2::ExifData& ed, Exiv2::IfdId ifdId)
    {
        ed.erase(std::remove_if(ed.begin(),
                                ed.end(),
                                Exiv2::FindExifdatum(ifdId)),
                 ed.end());
    }