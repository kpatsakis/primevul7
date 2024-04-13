    QByteArray PamData::getResponse(const struct pam_message* msg) {
        QByteArray response = findPrompt(msg).response;
        m_currentRequest.prompts.removeOne(findPrompt(msg));
        if (m_currentRequest.prompts.length() == 0)
            m_sent = false;
        return response;
    }