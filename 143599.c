  template<typename T>
  inline CImg<_cimg_Tfloat> operator-(const char *const expression, const CImg<T>& img) {
    return CImg<_cimg_Tfloat>(img,false).fill(expression,true)-=img;