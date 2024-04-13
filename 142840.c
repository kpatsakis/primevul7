    /**
       \param n Number of empty images to insert.
       \param pos Index of the insertion.
    **/
    CImgList<T>& insert(const unsigned int n, const unsigned int pos=~0U) {
      CImg<T> empty;
      if (!n) return *this;