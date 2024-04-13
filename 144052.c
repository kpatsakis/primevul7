    template<typename tp>
    CImg<T>& object3dtoCImg3d(const CImgList<tp>& primitives,
                              const bool full_check=true) {
      return get_object3dtoCImg3d(primitives,full_check).move_to(*this);