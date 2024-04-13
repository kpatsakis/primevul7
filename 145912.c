    long sumToLong(const Exiv2::Exifdatum& md)
    {
        long sum = 0;
        for (int i = 0; i < md.count(); ++i) {
            sum += md.toLong(i);
        }
        return sum;
    }