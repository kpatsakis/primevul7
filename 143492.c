    template<typename t>
    CImg(const t *const values, const unsigned int size_x, const unsigned int size_y=1,
         const unsigned int size_z=1, const unsigned int size_c=1, const bool is_shared=false):_is_shared(false) {
      if (is_shared) {
        _width = _height = _depth = _spectrum = 0; _data = 0;
        throw CImgArgumentException(_cimg_instance
                                    "CImg(): Invalid construction request of a (%u,%u,%u,%u) shared instance "
                                    "from a (%s*) buffer (pixel types are different).",
                                    cimg_instance,
                                    size_x,size_y,size_z,size_c,CImg<t>::pixel_type());
      }
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (values && siz) {
        _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);

        }
        const t *ptrs = values; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }