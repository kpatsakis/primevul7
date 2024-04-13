      static double max() { return DBL_MAX; }
      static double inf() {
#ifdef INFINITY
        return (double)INFINITY;
#else
        return max()*max();
#endif