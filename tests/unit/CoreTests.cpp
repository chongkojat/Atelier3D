#include "engine/core/Time.hpp"

#include "editor/commands/CommandStack.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <string>

using Editor::Commands::CommandStack;
using Engine::Core::Time;

TEST(Time, SecondsBetweenMeasuresDuration)
{
    const auto start = Time::Now();
    const auto end = start + std::chrono::milliseconds(1500);

    EXPECT_DOUBLE_EQ(Time::SecondsBetween(start, end), 1.5);
    EXPECT_DOUBLE_EQ(Time::SecondsBetween(end, start), -1.5);
}

TEST(Time, NowNeverGoesBackwards)
{
    const auto first = Time::Now();
    const auto second = Time::Now();
    EXPECT_GE(Time::SecondsBetween(first, second), 0.0);
}

TEST(CommandStack, ExecuteRunsDoButNotUndo)
{
    CommandStack commands;
    int value = 0;

    commands.Execute([&] { value = 1; }, [&] { value = -1; });

    EXPECT_EQ(value, 1);
}

TEST(CommandStack, UndoRunsInReverseOrder)
{
    CommandStack commands;
    std::string log;

    commands.Execute([&] { log += "a"; }, [&] { log += "A"; });
    commands.Execute([&] { log += "b"; }, [&] { log += "B"; });
    commands.Undo();
    commands.Undo();

    EXPECT_EQ(log, "abBA");
}

TEST(CommandStack, EachCommandIsUndoneOnlyOnce)
{
    CommandStack commands;
    int undoCount = 0;

    commands.Execute([] {}, [&] { ++undoCount; });
    commands.Undo();
    commands.Undo();

    EXPECT_EQ(undoCount, 1);
}

TEST(CommandStack, UndoOnEmptyStackIsSafe)
{
    CommandStack commands;
    EXPECT_NO_THROW(commands.Undo());
}
