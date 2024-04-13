    template<typename T>
    inline void sgels(char & TRANS, int &M, int &N, int &NRHS, T* lapA, int &LDA,
		      T* lapB, int &LDB, T* WORK, int &LWORK, int &INFO){
      dgels_(&TRANS, &M, &N, &NRHS, lapA, &LDA, lapB, &LDB, WORK, &LWORK, &INFO);