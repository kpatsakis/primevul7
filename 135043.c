GfxPatchMeshShading::GfxPatchMeshShading(int typeA, GfxPatch *patchesA, int nPatchesA, std::vector<std::unique_ptr<Function>> &&funcsA) : GfxShading(typeA), funcs(std::move(funcsA))
{
    patches = patchesA;
    nPatches = nPatchesA;
}