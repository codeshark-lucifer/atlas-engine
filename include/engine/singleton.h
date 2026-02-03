#pragma once
class Singleton
{
public:
    virtual ~Singleton() = default;

    static Singleton& Instance();
};
