    bool TiffReader::circularReference(const byte* start, IfdId group)
    {
        DirList::const_iterator pos = dirList_.find(start);
        if (pos != dirList_.end()) {
#ifndef SUPPRESS_WARNINGS
            EXV_ERROR << groupName(group) << " pointer references previously read "
                      << groupName(pos->second) << " directory; ignored.\n";
#endif
            return true;
        }
        dirList_[start] = group;
        return false;
    }