    template<typename tf, typename tc>
    const CImg<T>& save_off(const CImgList<tf>& primitives, const CImgList<tc>& colors,
                            const char *const filename) const {
      return _save_off(primitives,colors,0,filename);