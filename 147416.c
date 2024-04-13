qemuProcessPrepareHostHostdev(virDomainHostdevDefPtr hostdev)
{
    if (virHostdevIsSCSIDevice(hostdev)) {
        virDomainHostdevSubsysSCSIPtr scsisrc = &hostdev->source.subsys.u.scsi;

        switch ((virDomainHostdevSCSIProtocolType) scsisrc->protocol) {
        case VIR_DOMAIN_HOSTDEV_SCSI_PROTOCOL_TYPE_NONE: {
            virDomainHostdevSubsysSCSIHostPtr scsihostsrc = &scsisrc->u.host;
            virStorageSourcePtr src = scsisrc->u.host.src;
            g_autofree char *devstr = NULL;

            if (!(devstr = virSCSIDeviceGetSgName(NULL,
                                                  scsihostsrc->adapter,
                                                  scsihostsrc->bus,
                                                  scsihostsrc->target,
                                                  scsihostsrc->unit)))
                return -1;

            src->path = g_strdup_printf("/dev/%s", devstr);
            break;
        }

        case VIR_DOMAIN_HOSTDEV_SCSI_PROTOCOL_TYPE_ISCSI:
            break;

        case VIR_DOMAIN_HOSTDEV_SCSI_PROTOCOL_TYPE_LAST:
        default:
            virReportEnumRangeError(virDomainHostdevSCSIProtocolType, scsisrc->protocol);
            return -1;
        }
    }

    return 0;
}