    **/
    bool is_sameYZC(const unsigned int size_y, const unsigned int size_z, const unsigned int size_c) const {
      return is_sameYZ(size_y,size_z) && _spectrum==size_c;