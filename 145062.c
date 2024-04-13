  Config * Config::clone() const {
    return new Config(*this);
  }