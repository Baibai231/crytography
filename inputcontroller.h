#ifndef INPUTCONTROLLER_H
#define INPUTCONTROLLER_H

#include <QKeyEvent>
#include "Player.h"

class InputController {
public:
    InputController();

    void setPlayer(Player *p);

    void handleKeyPress(QKeyEvent *event);
    void handleKeyRelease(QKeyEvent *event);

private:
    Player *player = nullptr;
};

#endif
