    //! Rotate volumetric image with arbitrary angle and axis, around a center point \newinstance.
    CImg<T> get_rotate(const float u, const float v, const float w, const float angle,
                       const float cx, const float cy, const float cz,
                       const unsigned int interpolation=1, const unsigned int boundary_conditions=0) const {
      if (is_empty()) return *this;
      CImg<T> res(_width,_height,_depth,_spectrum);
      CImg<floatT> R = CImg<floatT>::rotation_matrix(u,v,w,-angle);
      _rotate(res,R,interpolation,boundary_conditions,cx,cy,cz,cx,cy,cz);
      return res;