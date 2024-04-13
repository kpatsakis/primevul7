GfxPatchMeshShading::GfxPatchMeshShading(const GfxPatchMeshShading *shading) : GfxShading(shading)
{
    nPatches = shading->nPatches;
    patches = (GfxPatch *)gmallocn(nPatches, sizeof(GfxPatch));
    memcpy(patches, shading->patches, nPatches * sizeof(GfxPatch));
    for (const auto &f : shading->funcs) {
        funcs.emplace_back(f->copy());
    }
}