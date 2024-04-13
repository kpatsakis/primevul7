
    inline void syev(char &JOB, char &UPLO, int &N, float *lapA, float *lapW, float *WORK, int &LWORK, int &INFO) {
      ssyev_(&JOB,&UPLO,&N,lapA,&N,lapW,WORK,&LWORK,&INFO);