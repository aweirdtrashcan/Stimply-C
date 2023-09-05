#include "core/application.h"

static void* app_state;

int main(void) {
    if (!application_start(800, 600, "Stimply Engine", &app_state)) {
        return -3;
    }

    application_run(app_state);

    application_destroy(app_state);

    app_state = 0;

    return 0;
}