    //! Load image from an ANALYZE7.5/NIFTI file \newinstance.
    static CImg<T> get_load_analyze(const char *const filename, float *const voxel_size=0) {
      return CImg<T>().load_analyze(filename,voxel_size);