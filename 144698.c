
    inline void getrf(int &N, float *lapA, int *IPIV, int &INFO) {
      sgetrf_(&N,&N,lapA,&N,IPIV,&INFO);