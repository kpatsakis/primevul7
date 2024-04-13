    //! Return specified image column \inplace.
    CImg<T>& column(const int x0) {
      return columns(x0,x0);