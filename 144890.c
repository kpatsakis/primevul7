    //! Return a reference to an empty list \const.
    static const CImgList<T>& const_empty() {
      static const CImgList<T> _empty;
      return _empty;