    **/
    const CImg<T>& save_inr(const char *const filename, const float *const voxel_size=0) const {
      return _save_inr(0,filename,voxel_size);