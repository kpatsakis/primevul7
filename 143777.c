
    T cubic_atXYZ_pc(const float fx, const float fy, const float fz, const int c) const {
      return cimg::type<T>::cut(cubic_atXYZ_p(fx,fy,fz,c));