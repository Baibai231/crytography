#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPixmap>
#include <QGraphicsItem>
#include <QVector>
#include <functional>

class Player : public QGraphicsPixmapItem {
public:
    Player();

    void advance(int phase) override;

    void moveLeft();
    void moveRight();
    void stopLeft();
    void stopRight();
    void jump();
    void forceKill();
    void respawn(qreal x, qreal y);

    void setDeadHandler(const std::function<void()> &handler);
    void setArrowRequestHandler(const std::function<void(qreal)> &handler);
    void setReachedDoorHandler(const std::function<void()> &handler);

    QRectF boundingRect() const override;


private:
    bool isDead = false;
    bool leftPressed = false;
    bool rightPressed = false;
    bool isGrounded = false;
    bool trapTriggered = false;
    bool jumpRequested = false;

    qreal xVelocity = 0;
    qreal yVelocity = 0;
    const qreal gravity = 0.6;
    const qreal jumpForce = -13;
    const qreal speed = 6;

    enum AnimationState {
        IdleState,
        RunState,
        JumpState
    };

    QPixmap idlePixmap;
    QVector<QPixmap> runFrames;
    QVector<QPixmap> jumpFrames;
    int currentFrame = 0;
    int jumpFrameIndex = 0;
    int frameCounter = 0;
    const int animationSpeed = 2;
    AnimationState animationState = IdleState;
    int jumpIgnoreTimer = 0;
    int jumpBufferTimer = 0;
    int coyoteTimer = 0;

    std::function<void()> deadHandler;
    std::function<void(qreal)> arrowRequestHandler;
    std::function<void()> reachedDoorHandler;

    bool isSolidSurface(QGraphicsItem *item) const;
    QGraphicsItem *findSupportingSurface() const;
    void updateAnimation();
};

#endif
