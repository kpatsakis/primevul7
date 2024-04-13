    template<typename T>
    inline T mod(const T& x, const T& m) {
      const double dx = (double)x, dm = (double)m;
      return (T)(dx - dm * std::floor(dx / dm));