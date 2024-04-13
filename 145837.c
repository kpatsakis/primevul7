    static inline bool inRange2(int value,int lo1,int hi1, int lo2,int hi2)
    {
        return inRange(lo1,value,hi1) || inRange(lo2,value,hi2);
    }