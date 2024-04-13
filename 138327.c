void LibarchivePlugin::slotRestoreWorkingDir()
{
    if (m_oldWorkingDir.isEmpty()) {
        return;
    }

    if (!QDir::setCurrent(m_oldWorkingDir)) {
        qCWarning(ARK) << "Failed to restore old working directory:" << m_oldWorkingDir;
    } else {
        m_oldWorkingDir.clear();
    }
}