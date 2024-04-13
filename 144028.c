      }
      static bool is_nan(const long double val) {
#ifdef isnan
        return (bool)isnan(val);
#else
        return !(val==val);
#endif