    **/
    CImg<T> get_tensor_at(const unsigned int x, const unsigned int y=0, const unsigned int z=0) const {
      const T *ptrs = data(x,y,z,0);
      const ulongT whd = (ulongT)_width*_height*_depth;
      if (_spectrum==6)
        return tensor(*ptrs,*(ptrs + whd),*(ptrs + 2*whd),*(ptrs + 3*whd),*(ptrs + 4*whd),*(ptrs + 5*whd));
      if (_spectrum==3)
        return tensor(*ptrs,*(ptrs + whd),*(ptrs + 2*whd));
      return tensor(*ptrs);