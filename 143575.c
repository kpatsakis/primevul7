    template<typename T>
    const CImgDisplay& snapshot(CImg<T>& img) const {
      cimg::unused(img);
      _no_display_exception();
      return *this;