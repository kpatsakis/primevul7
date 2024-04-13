    **/
    CImgList<T>& assign(const CImgDisplay &disp) {
      return assign(CImg<T>(disp));