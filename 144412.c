    **/
    CImgList<Tfloat> get_eigen() const {
      CImgList<Tfloat> res(2);
      eigen(res[0],res[1]);
      return res;