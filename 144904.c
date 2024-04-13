    **/
    CImg<T>& assign(const CImgDisplay &disp) {
      disp.snapshot(*this);
      return *this;