    const Prompt& PamData::findPrompt(const struct pam_message* msg) const {
        AuthPrompt::Type type = detectPrompt(msg);

        for (const Prompt &p : m_currentRequest.prompts) {
            if (type == p.type && p.message == QString::fromLocal8Bit(msg->msg))
                return p;
        }

        return invalidPrompt;
    }