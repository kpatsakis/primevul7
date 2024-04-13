GfxState::~GfxState()
{
    int i;

    if (fillColorSpace) {
        delete fillColorSpace;
    }
    if (strokeColorSpace) {
        delete strokeColorSpace;
    }
    if (fillPattern) {
        delete fillPattern;
    }
    if (strokePattern) {
        delete strokePattern;
    }
    for (i = 0; i < 4; ++i) {
        if (transfer[i]) {
            delete transfer[i];
        }
    }
    gfree(lineDash);
    if (path) {
        // this gets set to NULL by restore()
        delete path;
    }
    if (font) {
        font->decRefCnt();
    }
}