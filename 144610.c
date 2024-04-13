    template<typename t>
    CImg<T>& set_vector_at(const CImg<t>& vec, const unsigned int x, const unsigned int y=0, const unsigned int z=0) {
      if (x<_width && y<_height && z<_depth) {
        const t *ptrs = vec._data;
        const ulongT whd = (ulongT)_width*_height*_depth;
        T *ptrd = data(x,y,z);
        for (unsigned int k = std::min((unsigned int)vec.size(),_spectrum); k; --k) {
          *ptrd = (T)*(ptrs++); ptrd+=whd;
        }
      }
      return *this;