    //! Return specified range of image rows \inplace.
    CImg<T>& rows(const int y0, const int y1) {
      return get_rows(y0,y1).move_to(*this);