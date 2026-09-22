#pragma once

#include <functional>
#include <vector>

namespace Editor::Commands
{
    class CommandStack
    {
    public:
        void Execute(std::function<void()> doFn, std::function<void()> undoFn)
        {
            doFn();
            undo_.push_back(std::move(undoFn));
        }

        void Undo()
        {
            if (undo_.empty())
            {
                return;
            }

            auto fn = std::move(undo_.back());
            undo_.pop_back();
            fn();
        }

    private:
        std::vector<std::function<void()>> undo_;
    };
}
