
    T _cubic_atXY_pc(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(_cubic_atXY_p(fx,fy,z,c));