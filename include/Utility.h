#pragma once

class Utility : public Singleton<Utility> {
public:
    inline static bool first_run = true;
};
