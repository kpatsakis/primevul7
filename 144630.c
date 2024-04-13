    //! Load image from an INRIMAGE-4 file \overloading.
    CImg<T>& load_inr(std::FILE *const file, float *const voxel_size=0) {
      return _load_inr(file,0,voxel_size);