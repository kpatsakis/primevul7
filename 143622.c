    **/
    const CImg<T>& save_raw(const char *const filename, const bool is_multiplexed=false) const {
      return _save_raw(0,filename,is_multiplexed);