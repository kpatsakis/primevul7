
    T _cubic_atX_pc(const float fx, const int y, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atX_p(fx,y,z,c));