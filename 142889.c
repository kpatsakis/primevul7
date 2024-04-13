    template<typename tp, typename tc, typename to>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const CImgList<tc>& colors,
                              const to& opacities,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,colors,opacities,full_check).move_to(*this);