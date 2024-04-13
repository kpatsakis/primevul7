    **/
    const CImg<T>& save_raw(std::FILE *const file, const bool is_multiplexed=false) const {
      return _save_raw(file,0,is_multiplexed);