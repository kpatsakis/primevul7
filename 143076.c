    **/
    bool is_sameXZC(const unsigned int size_x, const unsigned int size_z, const unsigned int size_c) const {
      return is_sameXZ(size_x,size_z) && _spectrum==size_c;