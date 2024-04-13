    template<typename T>
    inline void syev(char &JOB, char &UPLO, int &N, T *lapA, T *lapW, T *WORK, int &LWORK, int &INFO) {
      dsyev_(&JOB,&UPLO,&N,lapA,&N,lapW,WORK,&LWORK,&INFO);