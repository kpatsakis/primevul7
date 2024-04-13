    // Lock/unlock mutex for CImg multi-thread programming.
    inline int mutex(const unsigned int n, const int lock_mode) {
      switch (lock_mode) {
      case 0 : cimg::Mutex_attr().unlock(n); return 0;
      case 1 : cimg::Mutex_attr().lock(n); return 0;
      default : return cimg::Mutex_attr().trylock(n);
      }