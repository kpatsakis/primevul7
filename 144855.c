
    inline void gesvd(char &JOB, int &M, int &N, float *lapA, int &MN,
                      float *lapS, float *lapU, float *lapV, float *WORK, int &LWORK, int &INFO) {
      sgesvd_(&JOB,&JOB,&M,&N,lapA,&MN,lapS,lapU,&M,lapV,&N,WORK,&LWORK,&INFO);