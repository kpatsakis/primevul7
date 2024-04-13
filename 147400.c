qemuProcessGraphicsSetupRenderNode(virDomainGraphicsDefPtr graphics,
                                   virQEMUCapsPtr qemuCaps)
{
    char **rendernode = NULL;

    if (!virDomainGraphicsNeedsAutoRenderNode(graphics))
        return 0;

    /* Don't bother picking a DRM node if QEMU doesn't support it. */
    if (graphics->type == VIR_DOMAIN_GRAPHICS_TYPE_SPICE) {
        if (!virQEMUCapsGet(qemuCaps, QEMU_CAPS_SPICE_RENDERNODE))
            return 0;

        rendernode = &graphics->data.spice.rendernode;
    } else {
        if (!virQEMUCapsGet(qemuCaps, QEMU_CAPS_EGL_HEADLESS_RENDERNODE))
            return 0;

        rendernode = &graphics->data.egl_headless.rendernode;
    }

    if (!(*rendernode = virHostGetDRMRenderNode()))
        return -1;

    return 0;
}