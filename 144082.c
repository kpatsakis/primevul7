    **/
    const CImg<T>& display(CImgDisplay& disp) const {
      disp.display(*this);
      return *this;