    template<typename t>
    CImg(const CImg<t>& img, const bool is_shared):_is_shared(false) {
      if (is_shared) {
        _width = _height = _depth = _spectrum = 0; _data = 0;
        throw CImgArgumentException(_cimg_instance
                                    "CImg(): Invalid construction request of a shared instance from a "
                                    "CImg<%s> image (%u,%u,%u,%u,%p) (pixel types are different).",
                                    cimg_instance,
                                    CImg<t>::pixel_type(),img._width,img._height,img._depth,img._spectrum,img._data);
      }
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        try { _data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                      img._width,img._height,img._depth,img._spectrum);
        }
        const t *ptrs = img._data; cimg_for(*this,ptrd,T) *ptrd = (T)*(ptrs++);
      } else { _width = _height = _depth = _spectrum = 0; _data = 0; }