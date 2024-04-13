    //! Return a shared-memory image referencing one channel of the image instance \const.
    const CImg<T> get_shared_channel(const unsigned int c0) const {
      return get_shared_channels(c0,c0);