    template<typename t>
    CImg<T>& assign(const t *const values, const unsigned int size_x, const unsigned int size_y,
                    const unsigned int size_z, const unsigned int size_c, const bool is_shared) {
      if (is_shared)
        throw CImgArgumentException(_cimg_instance
                                    "assign(): Invalid assignment request of shared instance from (%s*) buffer"
                                    "(pixel types are different).",
                                    cimg_instance,
                                    CImg<t>::pixel_type());
      return assign(values,size_x,size_y,size_z,size_c);