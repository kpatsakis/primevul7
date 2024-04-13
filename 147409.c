qemuProcessGraphicsAllocatePorts(virQEMUDriverPtr driver,
                                 virDomainGraphicsDefPtr graphics,
                                 bool allocate)
{
    virDomainGraphicsListenDefPtr glisten;

    if (graphics->nListens <= 0)
        return 0;

    glisten = &graphics->listens[0];

    if (glisten->type != VIR_DOMAIN_GRAPHICS_LISTEN_TYPE_ADDRESS &&
        glisten->type != VIR_DOMAIN_GRAPHICS_LISTEN_TYPE_NETWORK)
        return 0;

    switch (graphics->type) {
    case VIR_DOMAIN_GRAPHICS_TYPE_VNC:
        if (qemuProcessVNCAllocatePorts(driver, graphics, allocate) < 0)
            return -1;
        break;

    case VIR_DOMAIN_GRAPHICS_TYPE_SPICE:
        if (qemuProcessSPICEAllocatePorts(driver, graphics, allocate) < 0)
            return -1;
        break;

    case VIR_DOMAIN_GRAPHICS_TYPE_SDL:
    case VIR_DOMAIN_GRAPHICS_TYPE_RDP:
    case VIR_DOMAIN_GRAPHICS_TYPE_DESKTOP:
    case VIR_DOMAIN_GRAPHICS_TYPE_EGL_HEADLESS:
    case VIR_DOMAIN_GRAPHICS_TYPE_LAST:
        break;
    }

    return 0;
}