    Rational floatToRationalCast(float f)
    {
#if defined(_MSC_VER) && _MSC_VER < 1800
        if (!_finite(f)) {
#else
        if (!std::isfinite(f)) {
#endif
            return Rational(f > 0 ? 1 : -1, 0);
        }
        // Beware: primitive conversion algorithm
        int32_t den = 1000000;
        const long f_as_long = static_cast<long>(f);
        if (Safe::abs(f_as_long) > 2147) {
            den = 10000;
        }
        if (Safe::abs(f_as_long) > 214748) {
            den = 100;
        }
        if (Safe::abs(f_as_long) > 21474836) {
            den = 1;
        }
        const float rnd = f >= 0 ? 0.5f : -0.5f;
        const int32_t nom = static_cast<int32_t>(f * den + rnd);
        const int32_t g = gcd(nom, den);

        return Rational(nom / g, den / g);
    }

}                                       // namespace Exiv2