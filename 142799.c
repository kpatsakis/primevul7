    **/
    CImgList<Tfloat> get_symmetric_eigen() const {
      CImgList<Tfloat> res(2);
      symmetric_eigen(res[0],res[1]);
      return res;