    template<typename tf, typename tc>
    const CImg<T>& save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                            std::FILE *const file) const {
      return _save_off(primitives,colors,file,0);