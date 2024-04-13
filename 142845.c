    template<typename T>
    inline T _hypot(const T x, const T y) { // Slower but more precise version
      T nx = cimg::abs(x), ny = cimg::abs(y), t;
      if (nx<ny) { t = nx; nx = ny; } else t = ny;
      if (nx>0) { t/=nx; return nx*std::sqrt(1 + t*t); }
      return 0;