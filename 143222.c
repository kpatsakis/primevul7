    //! Return a reference to an empty image \const.
    static const CImg<T>& const_empty() {
      static const CImg<T> _empty;
      return _empty;