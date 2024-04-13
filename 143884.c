      return res;
    }

    //! Return a shared sublist \newinstance.
    const CImgList<T> get_shared_images(const unsigned int pos0, const unsigned int pos1) const {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "get_shared_images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);