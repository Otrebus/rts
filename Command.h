#pragma once

#include "Vector2.h"
#include <variant>
#include <deque>


struct BaseCommand
{
    bool active;
    BaseCommand() : active(false) {}
};

struct MoveCommand : public BaseCommand
{
    Vector2 destination;

    MoveCommand(Vector2 dest) : destination(dest) {}
};

struct ExtractCommand : public BaseCommand
{
    real radius;
    Vector2 destination;
    ExtractCommand(Vector2 dest, real radius) : destination(dest), radius(radius) {}
};

struct BuildCommand : public BaseCommand
{
    Vector2 destination;
    real width, height;

    BuildCommand(Vector2 dest, real width, real height) : destination(dest), width(width), height(height) {}
};

using Command = std::variant<MoveCommand, ExtractCommand, BuildCommand>;


class CommandQueue
{
    std::deque<Command> q;

public:
    void push(const Command& command)
    {
        q.push_back(command);
    }

    Command pop()
    {
        auto c = q.front();
        q.pop_front();
        return c;
    }

    void clear()
    {
        q.clear();
    }

    Command& front()
    {
        return q.front();
    }

    bool empty()
    {
        return q.empty();
    }

    std::vector<Command> get()
    {
        return std::vector<Command>(q.begin(), q.end());
    }
};