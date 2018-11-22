#include "peerMessageHandler.h"
#include "decoder.h"
#include "message.h"
#include "clientDatabase.h"
#include <syslog.h>
#include "fileHandler.h"

ChunkInfoResponse PeerMessageHandler::handleChunkInfoRequest(std::vector<char> b)
{
    ChunkInfoResponse res;
    Decoder decoder;
    std::shared_ptr<ChunkInfoRequest> m = std::make_shared<ChunkInfoRequest>(b);
    try
    {
        auto &database = ClientDatabase::getInstance();
        res.setHash(m->getHash());
        res.setChunkInfo(database.getmTorrent(m->getHash())->getBitChunks());
        res.setStatus("SUCCESS");
        return res;
    }
    catch (...)
    {
        //std::cout << "fail in info" << std::endl;
        ChunkInfoResponse res;
        res.setHash(m->getHash());
        res.setChunkInfo(std::vector<u_int32_t>(1, 1));
        res.setStatus("FAIL");
        return res;
    }
}

SendChunkResponse PeerMessageHandler::handlesendChunkRequest(std::vector<char> b)
{
    SendChunkResponse res;
    Decoder decoder;
    std::shared_ptr<SendChunkRequest> m = std::make_shared<SendChunkRequest>(b);
    try
    {
        auto &database = ClientDatabase::getInstance();
        auto path = database.getmTorrent(m->getHash())->getPath();
        std::vector<char> buf;
        FileHandler fHandler;

        fHandler.readFileChunk(m->getChunkId(), path, buf);
        res.setHash(m->getHash());
        res.setChunkIndex(m->getChunkId());
        res.setChunkData(std::move(buf));
        res.setStatus("SUCCESS");
        return res;
    }
    catch (...)
    {
        // std::cout << "fail" << std::endl;
        SendChunkResponse res;
        res.setHash(m->getHash());
        res.setChunkIndex(m->getChunkId());
        res.setStatus("FAIL");
        std::vector<char> b(1, 'a');
        res.setChunkData(b);
        return res;
    }
}