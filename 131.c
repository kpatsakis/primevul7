  virtual bool wifi_connecting() const {
    return wifi_ ? wifi_->connecting() : false;
  }
