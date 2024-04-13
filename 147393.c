qemuProcessStartValidateGraphics(virDomainObjPtr vm)
{
    size_t i;

    for (i = 0; i < vm->def->ngraphics; i++) {
        virDomainGraphicsDefPtr graphics = vm->def->graphics[i];

        switch (graphics->type) {
        case VIR_DOMAIN_GRAPHICS_TYPE_VNC:
        case VIR_DOMAIN_GRAPHICS_TYPE_SPICE:
            if (graphics->nListens > 1) {
                virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                               _("QEMU does not support multiple listens for "
                                 "one graphics device."));
                return -1;
            }
            break;

        case VIR_DOMAIN_GRAPHICS_TYPE_SDL:
        case VIR_DOMAIN_GRAPHICS_TYPE_RDP:
        case VIR_DOMAIN_GRAPHICS_TYPE_DESKTOP:
        case VIR_DOMAIN_GRAPHICS_TYPE_EGL_HEADLESS:
        case VIR_DOMAIN_GRAPHICS_TYPE_LAST:
            break;
        }
    }

    return 0;
}