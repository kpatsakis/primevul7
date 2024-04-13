    //! Return specified range of image channels \inplace.
    CImg<T>& channels(const int c0, const int c1) {
      return get_channels(c0,c1).move_to(*this);