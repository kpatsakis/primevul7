    **/
    CImgList<Tfloat> get_SVD(const bool sorting=true,
                             const unsigned int max_iteration=40, const float lambda=0) const {
      CImgList<Tfloat> res(3);
      SVD(res[0],res[1],res[2],sorting,max_iteration,lambda);
      return res;