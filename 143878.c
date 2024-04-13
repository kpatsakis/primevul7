    //! Convert pixel values from RGB to sRGB color spaces.
    CImg<T>& RGBtosRGB() {
      if (is_empty()) return *this;
      cimg_pragma_openmp(parallel for cimg_openmp_if_size(size(),32))
      cimg_rofoff(*this,off) {
        const Tfloat
          val = (Tfloat)_data[off]/255,
          sval = (Tfloat)(val<=0.0031308f?val*12.92f:1.055f*std::pow(val,0.416667f) - 0.055f);
        _data[off] = (T)cimg::cut(sval*255,0,255);
      }
      return *this;