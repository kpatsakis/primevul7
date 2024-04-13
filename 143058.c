    **/
    CImg<T>& load_analyze(const char *const filename, float *const voxel_size=0) {
      return _load_analyze(0,filename,voxel_size);