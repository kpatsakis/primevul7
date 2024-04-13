    **/
    bool is_sameXYZC(const unsigned int size_x, const unsigned int size_y,
                     const unsigned int size_z, const unsigned int size_c) const {
      return is_sameXYZ(size_x,size_y,size_z) && _spectrum==size_c;