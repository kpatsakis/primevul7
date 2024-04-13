    template<typename tp, typename tc, typename to>
    CImg<T> get_CImg3dtoobject3d(CImgList<tp>& primitives,
                                 CImgList<tc>& colors,
                                 CImgList<to>& opacities,
                                 const bool full_check=true) const {
      CImg<charT> error_message(1024);
      if (!is_CImg3d(full_check,error_message))
        throw CImgInstanceException(_cimg_instance
                                    "CImg3dtoobject3d(): image instance is not a CImg3d (%s).",
                                    cimg_instance,error_message.data());
      const T *ptrs = _data + 6;
      const unsigned int
        nb_points = cimg::float2uint((float)*(ptrs++)),
        nb_primitives = cimg::float2uint((float)*(ptrs++));
      const CImg<T> points = CImg<T>(ptrs,3,nb_points,1,1,true).get_transpose();
      ptrs+=3*nb_points;
      primitives.assign(nb_primitives);
      cimglist_for(primitives,p) {
        const unsigned int nb_inds = (unsigned int)*(ptrs++);
        primitives[p].assign(1,nb_inds);
        tp *ptrp = primitives[p]._data;
        for (unsigned int i = 0; i<nb_inds; ++i) *(ptrp++) = (tp)cimg::float2uint((float)*(ptrs++));
      }
      colors.assign(nb_primitives);
      cimglist_for(colors,c) {
        if (*ptrs==(T)-128) {
          ++ptrs;
          const unsigned int w = (unsigned int)*(ptrs++), h = (unsigned int)*(ptrs++), s = (unsigned int)*(ptrs++);
          if (!h && !s) colors[c].assign(colors[w],true);
          else { colors[c].assign(ptrs,w,h,1,s,false); ptrs+=w*h*s; }
        } else { colors[c].assign(ptrs,1,1,1,3,false); ptrs+=3; }
      }
      opacities.assign(nb_primitives);
      cimglist_for(opacities,o) {
        if (*ptrs==(T)-128) {
          ++ptrs;
          const unsigned int w = (unsigned int)*(ptrs++), h = (unsigned int)*(ptrs++), s = (unsigned int)*(ptrs++);
          if (!h && !s) opacities[o].assign(opacities[w],true);
          else { opacities[o].assign(ptrs,w,h,1,s,false); ptrs+=w*h*s; }
        } else opacities[o].assign(1,1,1,1,*(ptrs++));
      }
      return points;