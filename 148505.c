pk_transaction_error_quark (void)
{
	static GQuark quark = 0;
	if (!quark) {
		quark = g_quark_from_static_string ("pk-transaction-error-quark");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_DENIED,
					     PK_DBUS_INTERFACE_TRANSACTION ".Denied");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NOT_RUNNING,
					     PK_DBUS_INTERFACE_TRANSACTION ".NotRunning");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NO_ROLE,
					     PK_DBUS_INTERFACE_TRANSACTION ".NoRole");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_CANNOT_CANCEL,
					     PK_DBUS_INTERFACE_TRANSACTION ".CannotCancel");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NOT_SUPPORTED,
					     PK_DBUS_INTERFACE_TRANSACTION ".NotSupported");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NO_SUCH_TRANSACTION,
					     PK_DBUS_INTERFACE_TRANSACTION ".NoSuchTransaction");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NO_SUCH_FILE,
					     PK_DBUS_INTERFACE_TRANSACTION ".NoSuchFile");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NO_SUCH_DIRECTORY,
					     PK_DBUS_INTERFACE_TRANSACTION ".NoSuchDirectory");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_TRANSACTION_EXISTS_WITH_ROLE,
					     PK_DBUS_INTERFACE_TRANSACTION ".TransactionExistsWithRole");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_REFUSED_BY_POLICY,
					     PK_DBUS_INTERFACE_TRANSACTION ".RefusedByPolicy");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_PACKAGE_ID_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".PackageIdInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_SEARCH_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".SearchInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_SEARCH_PATH_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".PathInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_FILTER_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".FilterInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_INPUT_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".InputInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_INVALID_STATE,
					     PK_DBUS_INTERFACE_TRANSACTION ".InvalidState");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_INITIALIZE_FAILED,
					     PK_DBUS_INTERFACE_TRANSACTION ".InitializeFailed");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_COMMIT_FAILED,
					     PK_DBUS_INTERFACE_TRANSACTION ".CommitFailed");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_INVALID_PROVIDE,
					     PK_DBUS_INTERFACE_TRANSACTION ".InvalidProvide");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_PACK_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".PackInvalid");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_MIME_TYPE_NOT_SUPPORTED,
					     PK_DBUS_INTERFACE_TRANSACTION ".MimeTypeNotSupported");
		g_dbus_error_register_error (quark,
					     PK_TRANSACTION_ERROR_NUMBER_OF_PACKAGES_INVALID,
					     PK_DBUS_INTERFACE_TRANSACTION ".NumberOfPackagesInvalid");
	}
	return quark;
}