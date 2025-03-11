#pragma once

struct StaticFile
{
    const char *path;
    const char *contentType;
    const uint8_t *data;
    size_t size;
};