      _functor3d_float(const CImg<T>& pref):ref(pref) {}
      float operator()(const float x, const float y, const float z) const {
        return (float)ref._linear_atXYZ(x,y,z);