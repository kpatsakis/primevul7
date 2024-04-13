  bool AddCallback(GetUsageAndQuotaCallback* callback, bool unlimited) {
    if (unlimited)
      unlimited_callbacks_.push_back(callback);
    else
      callbacks_.push_back(callback);
    return (callbacks_.size() + unlimited_callbacks_.size() == 1);
  }
