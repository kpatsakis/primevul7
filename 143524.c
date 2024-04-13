    template<typename T>
    inline void getrs(char &TRANS, int &N, T *lapA, int *IPIV, T *lapB, int &INFO) {
      int one = 1;
      dgetrs_(&TRANS,&N,&one,lapA,&N,IPIV,lapB,&N,&INFO);