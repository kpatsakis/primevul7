    template<typename tp, typename tc>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const CImgList<tc>& colors,
                                      const bool full_check=true) const {
      CImgList<T> opacities;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);