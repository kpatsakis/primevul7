
    inline void getrs(char &TRANS, int &N, float *lapA, int *IPIV, float *lapB, int &INFO) {
      int one = 1;
      sgetrs_(&TRANS,&N,&one,lapA,&N,IPIV,lapB,&N,&INFO);