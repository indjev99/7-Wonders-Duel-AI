#pragma once

#include "listener.h"

#include <GLFW/glfw3.h>

struct ListenerGUI final : Listener
{
    ListenerGUI();
    ~ListenerGUI();

    void notifyStart() override;
    void notifyAction(const Action& action) override;
    void notifyEnd() override;

private:

    bool closed;
    GLFWwindow* window;

    void onClose();

    void drawState();
    void drawCard();
};
