    template<typename tp>
    CImg<floatT> get_object3dtoCImg3d(const CImgList<tp>& primitives,
                                      const bool full_check=true) const {
      CImgList<T> colors, opacities;
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check);