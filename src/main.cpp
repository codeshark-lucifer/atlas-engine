#include <engine/engine.hpp>

int WINDOW_WIDTH = 956, WINDOW_HEIGHT = 540;
const char *WINDOW_TITLE = "atlas - engine";

int main()
{
    Engine engine;
    engine.Initialize(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);


    engine.Run();
    return 0;
}
