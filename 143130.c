    //! Return specified image channel \inplace.
    CImg<T>& channel(const int c0) {
      return channels(c0,c0);