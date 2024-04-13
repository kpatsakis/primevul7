
    T _cubic_atXYZ_c(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(_cubic_atXYZ(fx,fy,fz,c));