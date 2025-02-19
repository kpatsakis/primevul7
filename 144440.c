    //! Return stream line of a 2D or 3D vector field.
    CImg<floatT> get_streamline(const float x, const float y, const float z,
                                const float L=256, const float dl=0.1f,
                                const unsigned int interpolation_type=2, const bool is_backward_tracking=false,
                                const bool is_oriented_only=false) const {
      if (_spectrum!=2 && _spectrum!=3)
        throw CImgInstanceException(_cimg_instance
                                    "streamline(): Instance is not a 2D or 3D vector field.",
                                    cimg_instance);
      if (_spectrum==2) {
        if (is_oriented_only) {
          typename CImg<T>::_functor4d_streamline2d_oriented func(*this);
          return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,true,
                            0,0,0,_width - 1.f,_height - 1.f,0.f);
        } else {
          typename CImg<T>::_functor4d_streamline2d_directed func(*this);
          return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,false,
                            0,0,0,_width - 1.f,_height - 1.f,0.f);
        }
      }
      if (is_oriented_only) {
        typename CImg<T>::_functor4d_streamline3d_oriented func(*this);
        return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,true,
                          0,0,0,_width - 1.f,_height - 1.f,_depth - 1.f);
      }
      typename CImg<T>::_functor4d_streamline3d_directed func(*this);
      return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,false,
                        0,0,0,_width - 1.f,_height - 1.f,_depth - 1.f);