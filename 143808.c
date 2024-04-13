    //! Rotate image with arbitrary angle, around a center point \newinstance.
    CImg<T> get_rotate(const float angle, const float cx, const float cy,
                       const unsigned int interpolation, const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res(_width,_height,_depth,_spectrum);
      _rotate(res,angle,interpolation,boundary_conditions,cx,cy,cx,cy);
      return res;