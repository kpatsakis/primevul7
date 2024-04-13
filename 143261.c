    **/
    CImg<T>& sort(const bool is_increasing=true, const char axis=0) {
      if (is_empty()) return *this;
      CImg<uintT> perm;
      switch (cimg::lowercase(axis)) {
      case 0 :
        _quicksort(0,size() - 1,perm,is_increasing,false);
        break;
      case 'x' : {
        perm.assign(_width);
        get_crop(0,0,0,0,_width - 1,0,0,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(perm[x],y,z,c);
      } break;
      case 'y' : {
        perm.assign(_height);
        get_crop(0,0,0,0,0,_height - 1,0,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,perm[y],z,c);
      } break;
      case 'z' : {
        perm.assign(_depth);
        get_crop(0,0,0,0,0,0,_depth - 1,0).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,y,perm[z],c);
      } break;
      case 'c' : {
        perm.assign(_spectrum);
        get_crop(0,0,0,0,0,0,0,_spectrum - 1).sort(perm,is_increasing);
        CImg<T> img(*this,false);
        cimg_forXYZC(*this,x,y,z,c) (*this)(x,y,z,c) = img(x,y,z,perm[c]);
      } break;
      default :
        throw CImgArgumentException(_cimg_instance
                                    "sort(): Invalid specified axis '%c' "
                                    "(should be { x | y | z | c }).",
                                    cimg_instance,axis);
      }
      return *this;