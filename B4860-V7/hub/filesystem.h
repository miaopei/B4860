#pragma once

class filesystem
{
public:
    filesystem() {}
    ~filesystem() {}


    static char buffer[1024];
    static uv_buf_t iov;


private:
    uv_fs_t open_req;
    uv_fs_t read_req;
    uv_fs_t write_req;
};

