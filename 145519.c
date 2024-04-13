        ~AutoLock()
        {
            if (xmpLockFct_) xmpLockFct_(pLockData_, false);
        }