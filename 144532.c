      _functor4d_streamline3d_directed(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z, const unsigned int c) const {
        return (float)ref._linear_atXYZ(x,y,z,c);