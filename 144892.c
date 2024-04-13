    template<typename tp, typename tc, typename to>
    CImg<T>& CImg3dtoobject3d(CImgList<tp>& primitives,
                              CImgList<tc>& colors,
                              CImgList<to>& opacities,
                              const bool full_check=true) {
      return get_CImg3dtoobject3d(primitives,colors,opacities,full_check).move_to(*this);