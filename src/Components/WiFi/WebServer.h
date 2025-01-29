#pragma once

class WebServer
{
public:
    // Gets the singleton instance
    static WebServer *Instance();

    // Initializes the asynchronous web server
    void Init();

private:
    // Handles the index API request
    String HandleIndexApiRequest();
};