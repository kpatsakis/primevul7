    template<typename t>
    CImg<T>& warp(const CImg<t>& p_warp, const unsigned int mode=0,
                  const unsigned int interpolation=1, const unsigned int boundary_conditions=0) {
      return get_warp(p_warp,mode,interpolation,boundary_conditions).move_to(*this);