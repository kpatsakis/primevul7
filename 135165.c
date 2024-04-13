GfxColorSpace *GfxPatternColorSpace::copy() const
{
    return new GfxPatternColorSpace(under ? under->copy() : nullptr);
}