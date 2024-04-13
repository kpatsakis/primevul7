      static cimg_int64 inf() { return max(); }
      static cimg_int64 cut(const double val) {
        return val<(double)min()?min():val>(double)max()?max():(cimg_int64)val;