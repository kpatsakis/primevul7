GfxSeparationColorSpace::~GfxSeparationColorSpace()
{
    delete name;
    delete alt;
    delete func;
    if (mapping != nullptr)
        gfree(mapping);
}