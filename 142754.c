    //! Resize image to become a symmetric tensor \newinstance.
    CImg<T> get_tensor() const {
      CImg<T> res;
      const ulongT siz = size();
      switch (siz) {
      case 1 : break;
      case 3 :
        res.assign(2,2);
        res(0,0) = (*this)(0);
        res(1,0) = res(0,1) = (*this)(1);
        res(1,1) = (*this)(2);
        break;
      case 6 :
        res.assign(3,3);
        res(0,0) = (*this)(0);
        res(1,0) = res(0,1) = (*this)(1);
        res(2,0) = res(0,2) = (*this)(2);
        res(1,1) = (*this)(3);
        res(2,1) = res(1,2) = (*this)(4);
        res(2,2) = (*this)(5);
        break;
      default :
        throw CImgInstanceException(_cimg_instance
                                    "tensor(): Invalid instance size (does not define a 1x1, 2x2 or 3x3 tensor).",
                                    cimg_instance);
      }
      return res;