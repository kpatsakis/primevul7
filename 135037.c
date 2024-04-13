GfxDeviceNColorSpace::~GfxDeviceNColorSpace()
{
    delete alt;
    delete func;
    for (auto entry : *sepsCS) {
        delete entry;
    }
    delete sepsCS;
    if (mapping != nullptr)
        gfree(mapping);
}