    **/
    CImgDisplay& assign(const unsigned int width, const unsigned int height,
                        const char *const title=0, const unsigned int normalization=3,
                        const bool is_fullscreen=false, const bool is_closed=false) {
      cimg::unused(width,height,title,normalization,is_fullscreen,is_closed);
      _no_display_exception();
      return assign();