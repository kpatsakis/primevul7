    template<typename t>
    CImg<T>& index(const CImg<t>& colormap, const float dithering=1, const bool map_indexes=false) {
      return get_index(colormap,dithering,map_indexes).move_to(*this);