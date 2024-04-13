        bool operator()(const Exiv2::Iptcdatum& iptcdatum) const
        {
            return dataset_ == iptcdatum.tag() && record_ == iptcdatum.record();
        }