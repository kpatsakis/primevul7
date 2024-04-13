    bool cmpIptcdataByRecord(const Iptcdatum& lhs, const Iptcdatum& rhs)
    {
        return lhs.record() < rhs.record();
    }