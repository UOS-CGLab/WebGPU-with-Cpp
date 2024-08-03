#include "src/Application.h"

int main() {
    Application app;

    if (!app.Initialize()) {
        return 1;
    }

#ifdef __EMSCRIPTEN__
    // Equivalent of the main loop when using Emscripten:
	auto callback = [](void *arg) {
		Application* pApp = reinterpret_cast<Application*>(arg);
		pApp->MainLoop(); // 4. We can use the application object
	};
	emscripten_set_main_loop_arg(callback, &app, 0, true);
#else // __EMSCRIPTEN__
    while (app.IsRunning()) {
        app.MainLoop();
    }
#endif // __EMSCRIPTEN__

    return 0;
}

