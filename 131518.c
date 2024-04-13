static NTSTATUS dcesrv_interface_dnsserver_bind(struct dcesrv_connection_context *context,
					        const struct dcesrv_interface *iface)
{
	return dcesrv_interface_bind_require_integrity(context, iface);
}