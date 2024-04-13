    template<typename tf, typename tc>
    static CImg<T> get_load_off(CImgList<tf>& primitives, CImgList<tc>& colors, std::FILE *const file) {
      return CImg<T>().load_off(primitives,colors,file);