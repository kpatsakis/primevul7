    /**
      \param pos0 Starting index of the sublist.
      \param pos1 Ending index of the sublist.
    **/
    CImgList<T> get_shared_images(const unsigned int pos0, const unsigned int pos1) {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "get_shared_images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);