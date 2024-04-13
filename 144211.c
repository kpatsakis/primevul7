    //! Construct image with specified size and initialize pixel values from a memory buffer \specialization.
    CImg(const T *const values, const unsigned int size_x, const unsigned int size_y=1,
         const unsigned int size_z=1, const unsigned int size_c=1, const bool is_shared=false) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (values && siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c; _is_shared = is_shared;
        if (_is_shared) _data = const_cast<T*>(values);
        else {
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                        size_x,size_y,size_z,size_c);
          }
          std::memcpy(_data,values,siz*sizeof(T));
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }