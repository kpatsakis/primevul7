    **/
    CImg<T>& haar(const char axis, const bool invert=false, const unsigned int nb_scales=1) {
      return get_haar(axis,invert,nb_scales).move_to(*this);