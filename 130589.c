void ConnectDialogEdit::on_qleName_textEdited(const QString& name) {
	if (bCustomLabel) {
		// If empty, then reset to automatic label.
		// NOTE(nik@jnstw.us): You may be tempted to set qleName to qleServer, but that results in the odd
		// UI behavior that clearing the field doesn't clear it; it'll immediately equal qleServer. Instead,
		// leave it empty and let it update the next time qleServer updates. Code in accept will default it
		// to qleServer if it isn't updated beforehand.
		if (name.simplified().isEmpty()) {
			bCustomLabel = false;
		}
	} else {
		// If manually edited, set to Custom
		bCustomLabel = true;
	}
}