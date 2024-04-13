    template<typename t>
    CImg(const CImg<t>& img, const char *const dimensions):
      _width(0),_height(0),_depth(0),_spectrum(0),_is_shared(false),_data(0) {
      assign(img,dimensions);