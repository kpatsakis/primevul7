      _functor4d_int(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return (float)ref((int)x,(int)y,(int)z,c);