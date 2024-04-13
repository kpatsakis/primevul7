
    inline void getri(int &N, float *lapA, int *IPIV, float* WORK, int &LWORK, int &INFO) {
      sgetri_(&N,lapA,&N,IPIV,WORK,&LWORK,&INFO);