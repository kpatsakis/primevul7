    //! Save image as an INRIMAGE-4 file \overloading.
    const CImg<T>& save_inr(std::FILE *const file, const float *const voxel_size=0) const {
      return _save_inr(file,0,voxel_size);