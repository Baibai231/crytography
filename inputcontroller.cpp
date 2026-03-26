#include "InputController.h"

InputController::InputController() {}

void InputController::setPlayer(Player *p) {
    player = p;
}

void InputController::handleKeyPress(QKeyEvent *event) {
    if (!player || event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_A:
        player->moveLeft();
        break;

    case Qt::Key_D:
        player->moveRight();
        break;

    case Qt::Key_W:
        player->jump();
        break;

    default:
        break;
    }
}

void InputController::handleKeyRelease(QKeyEvent *event) {
    if (!player || event->isAutoRepeat()) return;

    switch (event->key()) {
    case Qt::Key_A:
        player->stopLeft();
        break;

    case Qt::Key_D:
        player->stopRight();
        break;

    default:
        break;
    }
}
