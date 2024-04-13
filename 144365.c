    template<typename T>
    inline void gesvd(char &JOB, int &M, int &N, T *lapA, int &MN,
                      T *lapS, T *lapU, T *lapV, T *WORK, int &LWORK, int &INFO) {
      dgesvd_(&JOB,&JOB,&M,&N,lapA,&MN,lapS,lapU,&M,lapV,&N,WORK,&LWORK,&INFO);