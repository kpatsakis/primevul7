    const std::string& Image::tagName(uint16_t tag)
    {
        if ( init_ ) {
            int idx;
            const TagInfo* ti ;
            for (ti = Internal::  mnTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal:: iopTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal:: gpsTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal:: ifdTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal::exifTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal:: mpfTagList(), idx = 0; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
            for (ti = Internal::Nikon1MakerNote::tagList(), idx = 0
                                                    ; ti[idx].tag_ != 0xffff; ++idx) tags_[ti[idx].tag_] = ti[idx].name_;
        }
        init_ = false;

        return tags_[tag] ;
    }