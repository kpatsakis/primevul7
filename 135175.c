GfxPatchMeshShading *GfxPatchMeshShading::parse(GfxResources *res, int typeA, Dict *dict, Stream *str, OutputDev *out, GfxState *state)
{
    GfxPatchMeshShading *shading;
    std::vector<std::unique_ptr<Function>> funcsA;
    int coordBits, compBits, flagBits;
    double xMin, xMax, yMin, yMax;
    double cMin[gfxColorMaxComps], cMax[gfxColorMaxComps];
    double xMul, yMul;
    double cMul[gfxColorMaxComps];
    GfxPatch *patchesA, *p;
    int nComps, nPatchesA, patchesSize, nPts, nColors;
    unsigned int flag;
    double x[16], y[16];
    unsigned int xi, yi;
    double c[4][gfxColorMaxComps];
    unsigned int ci;
    Object obj1;
    int i, j;

    obj1 = dict->lookup("BitsPerCoordinate");
    if (obj1.isInt()) {
        coordBits = obj1.getInt();
    } else {
        error(errSyntaxWarning, -1, "Missing or invalid BitsPerCoordinate in shading dictionary");
        return nullptr;
    }
    if (unlikely(coordBits <= 0)) {
        error(errSyntaxWarning, -1, "Invalid BitsPerCoordinate in shading dictionary");
        return nullptr;
    }
    obj1 = dict->lookup("BitsPerComponent");
    if (obj1.isInt()) {
        compBits = obj1.getInt();
    } else {
        error(errSyntaxWarning, -1, "Missing or invalid BitsPerComponent in shading dictionary");
        return nullptr;
    }
    if (unlikely(compBits <= 0 || compBits > 31)) {
        error(errSyntaxWarning, -1, "Invalid BitsPerComponent in shading dictionary");
        return nullptr;
    }
    obj1 = dict->lookup("BitsPerFlag");
    if (obj1.isInt()) {
        flagBits = obj1.getInt();
    } else {
        error(errSyntaxWarning, -1, "Missing or invalid BitsPerFlag in shading dictionary");
        return nullptr;
    }
    obj1 = dict->lookup("Decode");
    if (obj1.isArray() && obj1.arrayGetLength() >= 6) {
        bool decodeOk = true;
        xMin = obj1.arrayGet(0).getNum(&decodeOk);
        xMax = obj1.arrayGet(1).getNum(&decodeOk);
        xMul = (xMax - xMin) / (pow(2.0, coordBits) - 1);
        yMin = obj1.arrayGet(2).getNum(&decodeOk);
        yMax = obj1.arrayGet(3).getNum(&decodeOk);
        yMul = (yMax - yMin) / (pow(2.0, coordBits) - 1);
        for (i = 0; 5 + 2 * i < obj1.arrayGetLength() && i < gfxColorMaxComps; ++i) {
            cMin[i] = obj1.arrayGet(4 + 2 * i).getNum(&decodeOk);
            cMax[i] = obj1.arrayGet(5 + 2 * i).getNum(&decodeOk);
            cMul[i] = (cMax[i] - cMin[i]) / (double)((1u << compBits) - 1);
        }
        nComps = i;

        if (!decodeOk) {
            error(errSyntaxWarning, -1, "Missing or invalid Decode array in shading dictionary");
            return nullptr;
        }
    } else {
        error(errSyntaxWarning, -1, "Missing or invalid Decode array in shading dictionary");
        return nullptr;
    }

    obj1 = dict->lookup("Function");
    if (!obj1.isNull()) {
        if (obj1.isArray()) {
            const int nFuncsA = obj1.arrayGetLength();
            if (nFuncsA > gfxColorMaxComps) {
                error(errSyntaxWarning, -1, "Invalid Function array in shading dictionary");
                return nullptr;
            }
            for (i = 0; i < nFuncsA; ++i) {
                Object obj2 = obj1.arrayGet(i);
                Function *f = Function::parse(&obj2);
                if (!f) {
                    return nullptr;
                }
                funcsA.emplace_back(f);
            }
        } else {
            Function *f = Function::parse(&obj1);
            if (!f) {
                return nullptr;
            }
            funcsA.emplace_back(f);
        }
    }

    nPatchesA = 0;
    patchesA = nullptr;
    patchesSize = 0;
    auto bitBuf = std::make_unique<GfxShadingBitBuf>(str);
    while (true) {
        if (!bitBuf->getBits(flagBits, &flag)) {
            break;
        }
        if (typeA == 6) {
            switch (flag) {
            case 0:
                nPts = 12;
                nColors = 4;
                break;
            case 1:
            case 2:
            case 3:
            default:
                nPts = 8;
                nColors = 2;
                break;
            }
        } else {
            switch (flag) {
            case 0:
                nPts = 16;
                nColors = 4;
                break;
            case 1:
            case 2:
            case 3:
            default:
                nPts = 12;
                nColors = 2;
                break;
            }
        }
        for (i = 0; i < nPts; ++i) {
            if (!bitBuf->getBits(coordBits, &xi) || !bitBuf->getBits(coordBits, &yi)) {
                break;
            }
            x[i] = xMin + xMul * (double)xi;
            y[i] = yMin + yMul * (double)yi;
        }
        if (i < nPts) {
            break;
        }
        for (i = 0; i < nColors; ++i) {
            for (j = 0; j < nComps; ++j) {
                if (!bitBuf->getBits(compBits, &ci)) {
                    break;
                }
                c[i][j] = cMin[j] + cMul[j] * (double)ci;
                if (funcsA.empty()) {
                    // ... and colorspace values can also be stored into doubles.
                    // They will be casted later.
                    c[i][j] = dblToCol(c[i][j]);
                }
            }
            if (j < nComps) {
                break;
            }
        }
        if (i < nColors) {
            break;
        }
        if (nPatchesA == patchesSize) {
            int oldPatchesSize = patchesSize;
            patchesSize = (patchesSize == 0) ? 16 : 2 * patchesSize;
            patchesA = (GfxPatch *)greallocn_checkoverflow(patchesA, patchesSize, sizeof(GfxPatch));
            if (unlikely(!patchesA)) {
                return nullptr;
            }
            memset(patchesA + oldPatchesSize, 0, (patchesSize - oldPatchesSize) * sizeof(GfxPatch));
        }
        p = &patchesA[nPatchesA];
        if (typeA == 6) {
            switch (flag) {
            case 0:
                p->x[0][0] = x[0];
                p->y[0][0] = y[0];
                p->x[0][1] = x[1];
                p->y[0][1] = y[1];
                p->x[0][2] = x[2];
                p->y[0][2] = y[2];
                p->x[0][3] = x[3];
                p->y[0][3] = y[3];
                p->x[1][3] = x[4];
                p->y[1][3] = y[4];
                p->x[2][3] = x[5];
                p->y[2][3] = y[5];
                p->x[3][3] = x[6];
                p->y[3][3] = y[6];
                p->x[3][2] = x[7];
                p->y[3][2] = y[7];
                p->x[3][1] = x[8];
                p->y[3][1] = y[8];
                p->x[3][0] = x[9];
                p->y[3][0] = y[9];
                p->x[2][0] = x[10];
                p->y[2][0] = y[10];
                p->x[1][0] = x[11];
                p->y[1][0] = y[11];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = c[0][j];
                    p->color[0][1].c[j] = c[1][j];
                    p->color[1][1].c[j] = c[2][j];
                    p->color[1][0].c[j] = c[3][j];
                }
                break;
            case 1:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[0][3];
                p->y[0][0] = patchesA[nPatchesA - 1].y[0][3];
                p->x[0][1] = patchesA[nPatchesA - 1].x[1][3];
                p->y[0][1] = patchesA[nPatchesA - 1].y[1][3];
                p->x[0][2] = patchesA[nPatchesA - 1].x[2][3];
                p->y[0][2] = patchesA[nPatchesA - 1].y[2][3];
                p->x[0][3] = patchesA[nPatchesA - 1].x[3][3];
                p->y[0][3] = patchesA[nPatchesA - 1].y[3][3];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[0][1].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[1][1].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            case 2:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[3][3];
                p->y[0][0] = patchesA[nPatchesA - 1].y[3][3];
                p->x[0][1] = patchesA[nPatchesA - 1].x[3][2];
                p->y[0][1] = patchesA[nPatchesA - 1].y[3][2];
                p->x[0][2] = patchesA[nPatchesA - 1].x[3][1];
                p->y[0][2] = patchesA[nPatchesA - 1].y[3][1];
                p->x[0][3] = patchesA[nPatchesA - 1].x[3][0];
                p->y[0][3] = patchesA[nPatchesA - 1].y[3][0];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[1][1].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[1][0].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            case 3:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[3][0];
                p->y[0][0] = patchesA[nPatchesA - 1].y[3][0];
                p->x[0][1] = patchesA[nPatchesA - 1].x[2][0];
                p->y[0][1] = patchesA[nPatchesA - 1].y[2][0];
                p->x[0][2] = patchesA[nPatchesA - 1].x[1][0];
                p->y[0][2] = patchesA[nPatchesA - 1].y[1][0];
                p->x[0][3] = patchesA[nPatchesA - 1].x[0][0];
                p->y[0][3] = patchesA[nPatchesA - 1].y[0][0];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[1][0].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[0][0].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            }
        } else {
            switch (flag) {
            case 0:
                p->x[0][0] = x[0];
                p->y[0][0] = y[0];
                p->x[0][1] = x[1];
                p->y[0][1] = y[1];
                p->x[0][2] = x[2];
                p->y[0][2] = y[2];
                p->x[0][3] = x[3];
                p->y[0][3] = y[3];
                p->x[1][3] = x[4];
                p->y[1][3] = y[4];
                p->x[2][3] = x[5];
                p->y[2][3] = y[5];
                p->x[3][3] = x[6];
                p->y[3][3] = y[6];
                p->x[3][2] = x[7];
                p->y[3][2] = y[7];
                p->x[3][1] = x[8];
                p->y[3][1] = y[8];
                p->x[3][0] = x[9];
                p->y[3][0] = y[9];
                p->x[2][0] = x[10];
                p->y[2][0] = y[10];
                p->x[1][0] = x[11];
                p->y[1][0] = y[11];
                p->x[1][1] = x[12];
                p->y[1][1] = y[12];
                p->x[1][2] = x[13];
                p->y[1][2] = y[13];
                p->x[2][2] = x[14];
                p->y[2][2] = y[14];
                p->x[2][1] = x[15];
                p->y[2][1] = y[15];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = c[0][j];
                    p->color[0][1].c[j] = c[1][j];
                    p->color[1][1].c[j] = c[2][j];
                    p->color[1][0].c[j] = c[3][j];
                }
                break;
            case 1:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[0][3];
                p->y[0][0] = patchesA[nPatchesA - 1].y[0][3];
                p->x[0][1] = patchesA[nPatchesA - 1].x[1][3];
                p->y[0][1] = patchesA[nPatchesA - 1].y[1][3];
                p->x[0][2] = patchesA[nPatchesA - 1].x[2][3];
                p->y[0][2] = patchesA[nPatchesA - 1].y[2][3];
                p->x[0][3] = patchesA[nPatchesA - 1].x[3][3];
                p->y[0][3] = patchesA[nPatchesA - 1].y[3][3];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                p->x[1][1] = x[8];
                p->y[1][1] = y[8];
                p->x[1][2] = x[9];
                p->y[1][2] = y[9];
                p->x[2][2] = x[10];
                p->y[2][2] = y[10];
                p->x[2][1] = x[11];
                p->y[2][1] = y[11];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[0][1].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[1][1].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            case 2:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[3][3];
                p->y[0][0] = patchesA[nPatchesA - 1].y[3][3];
                p->x[0][1] = patchesA[nPatchesA - 1].x[3][2];
                p->y[0][1] = patchesA[nPatchesA - 1].y[3][2];
                p->x[0][2] = patchesA[nPatchesA - 1].x[3][1];
                p->y[0][2] = patchesA[nPatchesA - 1].y[3][1];
                p->x[0][3] = patchesA[nPatchesA - 1].x[3][0];
                p->y[0][3] = patchesA[nPatchesA - 1].y[3][0];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                p->x[1][1] = x[8];
                p->y[1][1] = y[8];
                p->x[1][2] = x[9];
                p->y[1][2] = y[9];
                p->x[2][2] = x[10];
                p->y[2][2] = y[10];
                p->x[2][1] = x[11];
                p->y[2][1] = y[11];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[1][1].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[1][0].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            case 3:
                if (nPatchesA == 0) {
                    gfree(patchesA);
                    return nullptr;
                }
                p->x[0][0] = patchesA[nPatchesA - 1].x[3][0];
                p->y[0][0] = patchesA[nPatchesA - 1].y[3][0];
                p->x[0][1] = patchesA[nPatchesA - 1].x[2][0];
                p->y[0][1] = patchesA[nPatchesA - 1].y[2][0];
                p->x[0][2] = patchesA[nPatchesA - 1].x[1][0];
                p->y[0][2] = patchesA[nPatchesA - 1].y[1][0];
                p->x[0][3] = patchesA[nPatchesA - 1].x[0][0];
                p->y[0][3] = patchesA[nPatchesA - 1].y[0][0];
                p->x[1][3] = x[0];
                p->y[1][3] = y[0];
                p->x[2][3] = x[1];
                p->y[2][3] = y[1];
                p->x[3][3] = x[2];
                p->y[3][3] = y[2];
                p->x[3][2] = x[3];
                p->y[3][2] = y[3];
                p->x[3][1] = x[4];
                p->y[3][1] = y[4];
                p->x[3][0] = x[5];
                p->y[3][0] = y[5];
                p->x[2][0] = x[6];
                p->y[2][0] = y[6];
                p->x[1][0] = x[7];
                p->y[1][0] = y[7];
                p->x[1][1] = x[8];
                p->y[1][1] = y[8];
                p->x[1][2] = x[9];
                p->y[1][2] = y[9];
                p->x[2][2] = x[10];
                p->y[2][2] = y[10];
                p->x[2][1] = x[11];
                p->y[2][1] = y[11];
                for (j = 0; j < nComps; ++j) {
                    p->color[0][0].c[j] = patchesA[nPatchesA - 1].color[1][0].c[j];
                    p->color[0][1].c[j] = patchesA[nPatchesA - 1].color[0][0].c[j];
                    p->color[1][1].c[j] = c[0][j];
                    p->color[1][0].c[j] = c[1][j];
                }
                break;
            }
        }
        ++nPatchesA;
        bitBuf->flushBits();
    }

    if (typeA == 6) {
        for (i = 0; i < nPatchesA; ++i) {
            p = &patchesA[i];
            p->x[1][1] = (-4 * p->x[0][0] + 6 * (p->x[0][1] + p->x[1][0]) - 2 * (p->x[0][3] + p->x[3][0]) + 3 * (p->x[3][1] + p->x[1][3]) - p->x[3][3]) / 9;
            p->y[1][1] = (-4 * p->y[0][0] + 6 * (p->y[0][1] + p->y[1][0]) - 2 * (p->y[0][3] + p->y[3][0]) + 3 * (p->y[3][1] + p->y[1][3]) - p->y[3][3]) / 9;
            p->x[1][2] = (-4 * p->x[0][3] + 6 * (p->x[0][2] + p->x[1][3]) - 2 * (p->x[0][0] + p->x[3][3]) + 3 * (p->x[3][2] + p->x[1][0]) - p->x[3][0]) / 9;
            p->y[1][2] = (-4 * p->y[0][3] + 6 * (p->y[0][2] + p->y[1][3]) - 2 * (p->y[0][0] + p->y[3][3]) + 3 * (p->y[3][2] + p->y[1][0]) - p->y[3][0]) / 9;
            p->x[2][1] = (-4 * p->x[3][0] + 6 * (p->x[3][1] + p->x[2][0]) - 2 * (p->x[3][3] + p->x[0][0]) + 3 * (p->x[0][1] + p->x[2][3]) - p->x[0][3]) / 9;
            p->y[2][1] = (-4 * p->y[3][0] + 6 * (p->y[3][1] + p->y[2][0]) - 2 * (p->y[3][3] + p->y[0][0]) + 3 * (p->y[0][1] + p->y[2][3]) - p->y[0][3]) / 9;
            p->x[2][2] = (-4 * p->x[3][3] + 6 * (p->x[3][2] + p->x[2][3]) - 2 * (p->x[3][0] + p->x[0][3]) + 3 * (p->x[0][2] + p->x[2][0]) - p->x[0][0]) / 9;
            p->y[2][2] = (-4 * p->y[3][3] + 6 * (p->y[3][2] + p->y[2][3]) - 2 * (p->y[3][0] + p->y[0][3]) + 3 * (p->y[0][2] + p->y[2][0]) - p->y[0][0]) / 9;
        }
    }

    shading = new GfxPatchMeshShading(typeA, patchesA, nPatchesA, std::move(funcsA));
    if (!shading->init(res, dict, out, state)) {
        delete shading;
        return nullptr;
    }
    return shading;
}