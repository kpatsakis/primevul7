    template<typename t>
    CImg<T>& map(const CImg<t>& colormap, const unsigned int boundary_conditions=0) {
      return get_map(colormap,boundary_conditions).move_to(*this);