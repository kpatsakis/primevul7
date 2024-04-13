    //! Load image from an INRIMAGE-4 file \newinstance.
    static CImg<T> get_load_inr(std::FILE *const file, float *voxel_size=0) {
      return CImg<T>().load_inr(file,voxel_size);