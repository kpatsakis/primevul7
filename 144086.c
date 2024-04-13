    //! Advanced copy constructor \specialization.
    CImg(const CImg<T>& img, const bool is_shared) {
      const size_t siz = (size_t)img.size();
      if (img._data && siz) {
        _width = img._width; _height = img._height; _depth = img._depth; _spectrum = img._spectrum;
        _is_shared = is_shared;
        if (_is_shared) _data = const_cast<T*>(img._data);
        else {
          try { _data = new T[siz]; } catch (...) {
            _width = _height = _depth = _spectrum = 0; _data = 0;
            throw CImgInstanceException(_cimg_instance
                                        "CImg(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                        cimg_instance,
                                        cimg::strbuffersize(sizeof(T)*img._width*img._height*img._depth*img._spectrum),
                                        img._width,img._height,img._depth,img._spectrum);
          }
          std::memcpy(_data,img._data,siz*sizeof(T));
        }
      } else { _width = _height = _depth = _spectrum = 0; _is_shared = false; _data = 0; }