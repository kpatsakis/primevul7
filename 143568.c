    **/
    CImg<T>& operator=(const CImgDisplay& disp) {
      disp.snapshot(*this);
      return *this;