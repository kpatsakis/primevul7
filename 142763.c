    **/
    CImg<T> get_vector_at(const unsigned int x, const unsigned int y=0, const unsigned int z=0) const {
      CImg<T> res;
      if (res._height!=_spectrum) res.assign(1,_spectrum);
      const ulongT whd = (ulongT)_width*_height*_depth;
      const T *ptrs = data(x,y,z);
      T *ptrd = res._data;
      cimg_forC(*this,c) { *(ptrd++) = *ptrs; ptrs+=whd; }
      return res;