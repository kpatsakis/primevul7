      _functor4d_streamline2d_directed(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return c<2?(float)ref._linear_atXY(x,y,(int)z,c):0;