    }
    CImg<T>& operator=(CImg<T>&& img) {
      if (_is_shared) return assign(img);
      return img.swap(*this);