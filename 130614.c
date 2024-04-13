xps_free_font(xps_context_t *ctx, xps_font_t *font)
{
    if (font->font)
    {
        gs_font_finalize(ctx->memory, font->font);
        gs_free_object(ctx->memory, font->font, "font object");
    }
    xps_free(ctx, font);
}