  Config & Config::operator= (const Config & other)
  {
    del();
    copy(other);
    return *this;
  }