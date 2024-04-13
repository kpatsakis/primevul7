    **/
    CImg<T>& haar(const bool invert=false, const unsigned int nb_scales=1) {
      return get_haar(invert,nb_scales).move_to(*this);