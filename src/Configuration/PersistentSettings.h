#pragma once

namespace PersistentSettings
{
    void Init();

    void WriteOCPPServer(const String& server);
    String ReadOCPPServer();
};