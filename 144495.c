    //! Load image from a raw binary file \overloading.
    CImg<T>& load_raw(std::FILE *const file,
                      const unsigned int size_x=0, const unsigned int size_y=1,
                      const unsigned int size_z=1, const unsigned int size_c=1,
                      const bool is_multiplexed=false, const bool invert_endianness=false,
                      const ulongT offset=0) {
      return _load_raw(file,0,size_x,size_y,size_z,size_c,is_multiplexed,invert_endianness,offset);