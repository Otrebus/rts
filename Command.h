#pragma once

#include "Vector2.h"
#include <variant>
#include <queue>


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
};

using Command = std::variant<MoveCommand, ExtractCommand>;


class CommandQueue
{
    std::queue<Command> q;

public:
    void push(bool replace, const Command& command)
    {
        if(replace)
            q = std::queue<Command>();
        q.push(command);
    }

    Command pop()
    {
        auto c = q.front();
        q.pop();
        return c;
    }

    Command& front()
    {
        return q.front();
    }

    bool empty()
    {
        return q.empty();
    }
};