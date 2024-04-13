    **/
    CImg<T>& assign(const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (!siz) return assign();
      const size_t curr_siz = (size_t)size();
      if (siz!=curr_siz) {
	if (_is_shared)
          throw CImgArgumentException(_cimg_instance
                                      "assign(): Invalid assignement request of shared instance from specified "
                                      "image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      size_x,size_y,size_z,size_c);
	else {
          delete[] _data;
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "assign(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                        size_x,size_y,size_z,size_c);
          }
        }
      }
      _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
      return *this;