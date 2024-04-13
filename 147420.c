qemuProcessGraphicsReservePorts(virDomainGraphicsDefPtr graphics,
                                bool reconnect)
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
        if (!graphics->data.vnc.autoport ||
            reconnect) {
            if (virPortAllocatorSetUsed(graphics->data.vnc.port) < 0)
                return -1;
            graphics->data.vnc.portReserved = true;
        }
        if (graphics->data.vnc.websocket > 0 &&
            virPortAllocatorSetUsed(graphics->data.vnc.websocket) < 0)
            return -1;
        break;

    case VIR_DOMAIN_GRAPHICS_TYPE_SPICE:
        if (graphics->data.spice.autoport && !reconnect)
            return 0;

        if (graphics->data.spice.port > 0) {
            if (virPortAllocatorSetUsed(graphics->data.spice.port) < 0)
                return -1;
            graphics->data.spice.portReserved = true;
        }

        if (graphics->data.spice.tlsPort > 0) {
            if (virPortAllocatorSetUsed(graphics->data.spice.tlsPort) < 0)
                return -1;
            graphics->data.spice.tlsPortReserved = true;
        }
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