    **/
    static CImg<T> identity_matrix(const unsigned int N) {
      CImg<T> res(N,N,1,1,0);
      cimg_forX(res,x) res(x,x) = 1;
      return res;