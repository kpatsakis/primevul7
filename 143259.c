    template<typename T>
    CImgDisplay& assign(const CImg<T>& img,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      _no_display_exception();
      return assign(img._width,img._height,title,normalization,is_fullscreen,is_closed);