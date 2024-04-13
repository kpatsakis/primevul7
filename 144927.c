    template<typename tf, typename tc>
    CImg<T>& load_off(CImgList<tf>& primitives, CImgList<tc>& colors, std::FILE *const file) {
      return _load_off(primitives,colors,file,0);