jbig2_sd_glyph(Jbig2SymbolDict *dict, unsigned int id)
{
    if (dict == NULL)
        return NULL;
    return dict->glyphs[id];
}