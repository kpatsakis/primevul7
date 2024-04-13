    template<typename tf, typename tc>
    static CImg<T> get_load_off(CImgList<tf>& primitives, CImgList<tc>& colors, const char *const filename) {
      return CImg<T>().load_off(primitives,colors,filename);