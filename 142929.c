      _functor2d_float(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y) const {
        return (float)ref._linear_atXY(x,y);