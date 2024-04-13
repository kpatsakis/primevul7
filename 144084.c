    template<typename T>
    inline void getri(int &N, T *lapA, int *IPIV, T* WORK, int &LWORK, int &INFO) {
      dgetri_(&N,lapA,&N,IPIV,WORK,&LWORK,&INFO);