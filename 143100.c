    **/
    CImg(const unsigned int size_x, const unsigned int size_y,
         const unsigned int size_z, const unsigned int size_c, const T& value):
      _is_shared(false) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
        fill(value);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }