    template<typename T>
    inline void getrf(int &N, T *lapA, int *IPIV, int &INFO) {
      dgetrf_(&N,&N,lapA,&N,IPIV,&INFO);