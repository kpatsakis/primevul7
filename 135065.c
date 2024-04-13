bool GfxState::parseBlendMode(Object *obj, GfxBlendMode *mode)
{
    int i, j;

    if (obj->isName()) {
        for (i = 0; i < nGfxBlendModeNames; ++i) {
            if (!strcmp(obj->getName(), gfxBlendModeNames[i].name)) {
                *mode = gfxBlendModeNames[i].mode;
                return true;
            }
        }
        return false;
    } else if (obj->isArray()) {
        for (i = 0; i < obj->arrayGetLength(); ++i) {
            Object obj2 = obj->arrayGet(i);
            if (!obj2.isName()) {
                return false;
            }
            for (j = 0; j < nGfxBlendModeNames; ++j) {
                if (!strcmp(obj2.getName(), gfxBlendModeNames[j].name)) {
                    *mode = gfxBlendModeNames[j].mode;
                    return true;
                }
            }
        }
        *mode = gfxBlendNormal;
        return true;
    } else {
        return false;
    }
}