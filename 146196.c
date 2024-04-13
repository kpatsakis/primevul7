    void XmpData::eraseFamily(XmpData::iterator& pos)
    {
        // https://github.com/Exiv2/exiv2/issues/521
        // delete 'children' of XMP composites (XmpSeq and XmpBag)

        // I build a StringVector of keys to remove
        // Then I remove them with erase(....)
        // erase() has nasty side effects on its argument
        // The side effects are avoided by the two-step approach
        // https://github.com/Exiv2/exiv2/issues/560
        std::string         key(pos->key());
        Exiv2::StringVector keys;
        while ( pos != xmpMetadata_.end() ) {
            if ( pos->key().find(key)==0 ) {
                keys.push_back(pos->key());
                pos++;
            } else {
                break;
            }
        }
        // now erase the family!
        for ( Exiv2::StringVector_i it = keys.begin() ; it != keys.end() ; it++ ) {
            erase(findKey(Exiv2::XmpKey(*it)));
        }
    }