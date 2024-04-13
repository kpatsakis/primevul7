      return get_images(pos0,pos1).move_to(*this);
    }

    //! Return a sublist \newinstance.
    CImgList<T> get_images(const unsigned int pos0, const unsigned int pos1) const {
      if (pos0>pos1 || pos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "images(): Specified sub-list indices (%u->%u) are out of bounds.",
                                    cimglist_instance,
                                    pos0,pos1);