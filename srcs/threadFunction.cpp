#include "../headers/threadFunction.hpp"

void chunkThread(ChunkMap *chunks, GLFWwindow *window, int start_x, int end_x, int start_z, int end_z)
{
    while (!stop)
    {
        createChunk(chunks, start_x, end_x, start_z, end_z);
    }
}

void meshThread(ChunkMap *chunks, GLFWwindow *window, MeshMap *mesh)
{
    while (!stop)
    {
        mtx.lock();
        auto c = chunks->begin();
        auto f = chunks->end();
        mtx.unlock();

        if (c != f)
        {
            mtxc.lock();

            if (mesh->find(c->first) == mesh->end() && mesh->size() < 10)
            {
                mesh->emplace(PosMesh((c->first), new Mesh(createMesh(*c->second))));
                mtx.lock();
                chunks->find(c->first)->second->freeCubeData();
                delete chunks->find(c->first)->second;
                chunks->erase(c->first);
                mtx.unlock();
            }
        }
        mtxc.unlock();
        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}