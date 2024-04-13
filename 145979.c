        bool operator()(const Exiv2::Exifdatum& md) const
        {
            return idx_ == md.idx() && 0 == strcmp(md.groupName().c_str(), groupName_);
        }