        AutoLock(Exiv2::XmpParser::XmpLockFct xmpLockFct, void* pLockData)
            : xmpLockFct_(xmpLockFct), pLockData_(pLockData)
        {
            if (xmpLockFct_) xmpLockFct_(pLockData_, true);
        }