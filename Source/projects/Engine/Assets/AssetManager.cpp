#include "Engine/Assets/AssetManager.hpp"

namespace Engine::Assets
{
    void AssetManager::SetSearchPath(std::string path)
    {
        searchPath_ = std::move(path);
    }

    const std::string& AssetManager::GetSearchPath() const
    {
        return searchPath_;
    }

    void AssetManager::RegisterVirtualPath(std::string name, std::string relativePath)
    {
        virtualPaths_[std::move(name)] = std::move(relativePath);
    }

    std::string AssetManager::Resolve(std::string_view nameOrPath) const
    {
        const auto it = virtualPaths_.find(std::string{nameOrPath});
        if (it != virtualPaths_.end())
        {
            return searchPath_ + "/" + it->second;
        }

        return std::string{nameOrPath};
    }
}
