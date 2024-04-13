    **/
    T cubic_atXY_c(const float fx, const float fy, const int z, const int c) const {
      return cimg::type<T>::cut(cubic_atXY(fx,fy,z,c));