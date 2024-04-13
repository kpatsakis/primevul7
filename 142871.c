    //! Load image from an INRIMAGE-4 file \newinstance.
    static CImg<T> get_load_inr(const char *const filename, float *const voxel_size=0) {
      return CImg<T>().load_inr(filename,voxel_size);