    template<typename tp, typename tc>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const CImgList<tc>& colors,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,colors,full_check).move_to(*this);