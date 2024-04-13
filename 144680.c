
    inline void sgels(char & TRANS, int &M, int &N, int &NRHS, float* lapA, int &LDA,
		      float* lapB, int &LDB, float* WORK, int &LWORK, int &INFO){
      sgels_(&TRANS, &M, &N, &NRHS, lapA, &LDA, lapB, &LDB, WORK, &LWORK, &INFO);