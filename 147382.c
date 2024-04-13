qemuProcessGetNetworkAddress(const char *netname,
                             char **netaddr)
{
    g_autoptr(virConnect) conn = NULL;
    int ret = -1;
    g_autoptr(virNetwork) net = NULL;
    virNetworkDefPtr netdef = NULL;
    virNetworkIPDefPtr ipdef;
    virSocketAddr addr;
    virSocketAddrPtr addrptr = NULL;
    char *dev_name = NULL;
    g_autofree char *xml = NULL;

    *netaddr = NULL;

    if (!(conn = virGetConnectNetwork()))
        return -1;

    net = virNetworkLookupByName(conn, netname);
    if (!net)
        goto cleanup;

    xml = virNetworkGetXMLDesc(net, 0);
    if (!xml)
        goto cleanup;

    netdef = virNetworkDefParseString(xml, NULL);
    if (!netdef)
        goto cleanup;

    switch ((virNetworkForwardType) netdef->forward.type) {
    case VIR_NETWORK_FORWARD_NONE:
    case VIR_NETWORK_FORWARD_NAT:
    case VIR_NETWORK_FORWARD_ROUTE:
    case VIR_NETWORK_FORWARD_OPEN:
        ipdef = virNetworkDefGetIPByIndex(netdef, AF_UNSPEC, 0);
        if (!ipdef) {
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("network '%s' doesn't have an IP address"),
                           netdef->name);
            goto cleanup;
        }
        addrptr = &ipdef->address;
        break;

    case VIR_NETWORK_FORWARD_BRIDGE:
        if ((dev_name = netdef->bridge))
            break;
        /*
         * fall through if netdef->bridge wasn't set, since that is
         * macvtap bridge mode network.
         */
        G_GNUC_FALLTHROUGH;

    case VIR_NETWORK_FORWARD_PRIVATE:
    case VIR_NETWORK_FORWARD_VEPA:
    case VIR_NETWORK_FORWARD_PASSTHROUGH:
        if ((netdef->forward.nifs > 0) && netdef->forward.ifs)
            dev_name = netdef->forward.ifs[0].device.dev;

        if (!dev_name) {
            virReportError(VIR_ERR_INTERNAL_ERROR,
                           _("network '%s' has no associated interface or bridge"),
                           netdef->name);
            goto cleanup;
        }
        break;

    case VIR_NETWORK_FORWARD_HOSTDEV:
        break;

    case VIR_NETWORK_FORWARD_LAST:
    default:
        virReportEnumRangeError(virNetworkForwardType, netdef->forward.type);
        goto cleanup;
    }

    if (dev_name) {
        if (virNetDevIPAddrGet(dev_name, &addr) < 0)
            goto cleanup;
        addrptr = &addr;
    }

    if (!(addrptr &&
          (*netaddr = virSocketAddrFormat(addrptr)))) {
        goto cleanup;
    }

    ret = 0;
 cleanup:
    virNetworkDefFree(netdef);
    return ret;
}