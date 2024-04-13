        bool operator()(const Exiv2::Exifdatum& exifdatum) const
        {
            return key_ == exifdatum.key();
        }