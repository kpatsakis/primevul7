      _functor2d_int(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y) const {
        return (float)ref((int)x,(int)y);