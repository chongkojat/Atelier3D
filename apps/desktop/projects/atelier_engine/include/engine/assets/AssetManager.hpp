#pragma once

#include <string>
#include <unordered_map>

namespace Engine::Assets
{
    class AssetManager
    {
    public:
        void SetSearchPath(std::string path);
        const std::string& GetSearchPath() const;

        void RegisterVirtualPath(std::string name, std::string relativePath);
        std::string Resolve(std::string_view nameOrPath) const;

    private:
        std::string searchPath_ = ".";
        std::unordered_map<std::string, std::string> virtualPaths_;
    };
}
