    //! Compute image Laplacian \newinstance.
    CImg<Tfloat> get_laplacian() const {
      if (is_empty()) return CImg<Tfloat>();
      CImg<Tfloat> res(_width,_height,_depth,_spectrum);
      if (_depth>1) { // 3D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height*_depth>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3x3(I,Tfloat);
          cimg_for3x3x3(*this,x,y,z,c,I,Tfloat) *(ptrd++) = Incc + Ipcc + Icnc + Icpc + Iccn + Iccp - 6*Iccc;
        }
      } else if (_height>1) { // 2D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width*_height>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _depth*_spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,0,c,I,Tfloat) *(ptrd++) = Inc + Ipc + Icn + Icp - 4*Icc;
        }
      } else { // 1D
        cimg_pragma_openmp(parallel for cimg_openmp_if(_width>=(cimg_openmp_sizefactor)*1048576 &&
                                                       _height*_depth*_spectrum>=2))
        cimg_forC(*this,c) {
          Tfloat *ptrd = res.data(0,0,0,c);
          CImg_3x3(I,Tfloat);
          cimg_for3x3(*this,x,y,0,c,I,Tfloat) *(ptrd++) = Inc + Ipc - 2*Icc;
        }
      }
      return res;