qemuProcessVNCAllocatePorts(virQEMUDriverPtr driver,
                            virDomainGraphicsDefPtr graphics,
                            bool allocate)
{
    unsigned short port;

    if (!allocate) {
        if (graphics->data.vnc.autoport)
            graphics->data.vnc.port = 5900;

        return 0;
    }

    if (graphics->data.vnc.autoport) {
        if (virPortAllocatorAcquire(driver->remotePorts, &port) < 0)
            return -1;
        graphics->data.vnc.port = port;
    }

    if (graphics->data.vnc.websocket == -1) {
        if (virPortAllocatorAcquire(driver->webSocketPorts, &port) < 0)
            return -1;
        graphics->data.vnc.websocket = port;
        graphics->data.vnc.websocketGenerated = true;
    }

    return 0;
}