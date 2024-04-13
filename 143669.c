    template<typename tf, typename tc>
    CImg<T>& load_off(CImgList<tf>& primitives, CImgList<tc>& colors, const char *const filename) {
      return _load_off(primitives,colors,0,filename);