  void set_date_cache(DateCache* date_cache) {
    if (date_cache != date_cache_) {
      delete date_cache_;
    }
    date_cache_ = date_cache;
  }