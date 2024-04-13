    //! Convert pixel values from sRGB to RGB color spaces.
    CImg<T>& sRGBtoRGB() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32))
      cimg_rofoff(*this,off) {
        const Tfloat
          sval = (Tfloat)_data[off]/255,
          val = (Tfloat)(sval<=0.04045f?sval/12.92f:std::pow((sval + 0.055f)/(1.055f),2.4f));
        _data[off] = (T)cimg::cut(val*255,0,255);
      }
      return *this;