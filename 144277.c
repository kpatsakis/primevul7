    template<typename T>
    CImgDisplay& assign(const CImgList<T>& list,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      _no_display_exception();
      return assign(list._width,list._width,title,normalization,is_fullscreen,is_closed);