    **/
    CImg<T>& load_inr(const char *const filename, float *const voxel_size=0) {
      return _load_inr(0,filename,voxel_size);