#include "Player.h"
#include <QGraphicsScene>
#include <QRectF>
#include <QTransform>

Player::Player() {
    runFrames.clear();
    jumpFrames.clear();
    for (int i = 0; i < 12; ++i) {
        QString fileName = QString(":/images/run_%1.png").arg(i);
        QPixmap map(fileName);
        if (!map.isNull()) {
            QPixmap scaled = map.scaled(60, 60, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            if (i == 0) {
                idlePixmap = scaled;
            }
            if (i <= 6) {
                runFrames.append(scaled);
            } else {
                jumpFrames.append(scaled);
            }
        }
    }

    if (idlePixmap.isNull()) {
        idlePixmap = QPixmap(40, 40);
        idlePixmap.fill(Qt::red);
    }
    if (runFrames.isEmpty()) {
        runFrames.append(idlePixmap);
    }
    if (jumpFrames.isEmpty()) {
        jumpFrames.append(idlePixmap);
    }

    setPixmap(idlePixmap);
    setTransformOriginPoint(30, 30);
    setData(0, "Player");
}

void Player::moveLeft() { leftPressed = true; }
void Player::moveRight() { rightPressed = true; }
void Player::stopLeft() { leftPressed = false; }
void Player::stopRight() { rightPressed = false; }

void Player::setDeadHandler(const std::function<void()> &handler) {
    deadHandler = handler;
}

void Player::setArrowRequestHandler(const std::function<void(qreal)> &handler) {
    arrowRequestHandler = handler;
}

void Player::setReachedDoorHandler(const std::function<void()> &handler) {
    reachedDoorHandler = handler;
}

bool Player::isSolidSurface(QGraphicsItem *item) const {
    if (!item) return false;
    const QString type = item->data(0).toString();
    return type == "Ground" || type == "Trap" || type.isEmpty();
}

QGraphicsItem *Player::findSupportingSurface() const {
    if (!scene()) return nullptr;

    const QRectF selfRect = sceneBoundingRect();
    const QRectF feetRect(selfRect.left() + 6, selfRect.bottom() + 1, selfRect.width() - 12, 4);
    const QList<QGraphicsItem *> itemsBelow = scene()->items(feetRect, Qt::IntersectsItemBoundingRect, Qt::DescendingOrder, transform());
    QGraphicsItem *bestSupport = nullptr;
    qreal bestOverlap = 0;

    for (QGraphicsItem *item : itemsBelow) {
        if (item == this || !isSolidSurface(item)) {
            continue;
        }

        const QRectF itemRect = item->sceneBoundingRect();
        const qreal overlap = qMin(feetRect.right(), itemRect.right()) - qMax(feetRect.left(), itemRect.left());
        const qreal verticalGap = itemRect.top() - selfRect.bottom();

        if (overlap >= 12 && verticalGap >= -1 && verticalGap <= 6 && overlap > bestOverlap) {
            bestOverlap = overlap;
            bestSupport = item;
        }
    }

    return bestSupport;
}

void Player::updateAnimation() {
    AnimationState targetState = IdleState;
    if (!isGrounded) {
        targetState = JumpState;
    } else if (xVelocity != 0) {
        targetState = RunState;
    }

    if (animationState != targetState) {
        animationState = targetState;
        currentFrame = 0;
        jumpFrameIndex = 0;
        frameCounter = 0;
    }

    if (animationState == JumpState) {
        frameCounter++;
        if (frameCounter >= animationSpeed) {
            frameCounter = 0;
            if (jumpFrameIndex < jumpFrames.size() - 1) {
                jumpFrameIndex++;
            }
        }
        setPixmap(jumpFrames[jumpFrameIndex]);
        return;
    }

    if (animationState == RunState) {
        frameCounter++;
        if (frameCounter >= animationSpeed) {
            frameCounter = 0;
            currentFrame = (currentFrame + 1) % runFrames.size();
        }
        setPixmap(runFrames[currentFrame]);
        return;
    }

    setPixmap(idlePixmap);
}

void Player::jump() {
    jumpRequested = true;
    jumpBufferTimer = 12;

    if (!isDead && (isGrounded || coyoteTimer > 0)) {
        yVelocity = jumpForce;
        isGrounded = false;
        jumpIgnoreTimer = 6;
        jumpBufferTimer = 0;
        jumpRequested = false;
        coyoteTimer = 0;
        updateAnimation();
    }
}

void Player::advance(int phase) {
    if (!phase || isDead) return;

    // ========= 状态更新 =========
    if (jumpIgnoreTimer > 0) jumpIgnoreTimer--;
    if (jumpBufferTimer > 0) jumpBufferTimer--;
    else jumpRequested = false;
    if (!isGrounded && coyoteTimer > 0) coyoteTimer--;

    // ========= 水平移动 =========
    xVelocity = 0;
    if (leftPressed) {
        xVelocity = -speed;
        setTransform(QTransform().scale(-1, 1));
    } else if (rightPressed) {
        xVelocity = speed;
        setTransform(QTransform());
    }

    if (xVelocity != 0) {
        qreal prevX = x();
        qreal nextX = x() + xVelocity;

        if (scene()) {
            nextX = qBound(0.0, nextX,
                           scene()->sceneRect().width() - boundingRect().width());
        }

        setPos(nextX, y());

        for (QGraphicsItem* item : collidingItems()) {
            if (!isSolidSurface(item)) continue;

            QRectF a = sceneBoundingRect();
            QRectF b = item->sceneBoundingRect();

            qreal overlap = qMin(a.bottom(), b.bottom()) - qMax(a.top(), b.top());
            if (overlap > 10) {
                setPos(prevX, y());
                xVelocity = 0;
                break;
            }
        }
    }

    // ========= 垂直移动 =========
    yVelocity += gravity;
    setPos(x(), y() + yVelocity);

    QRectF selfRect = sceneBoundingRect();

    // ========= 脚底区域 =========
    QRectF feetRect(
        selfRect.left() + selfRect.width() * 0.2,
        selfRect.bottom() - 2,
        selfRect.width() * 0.6,
        4
        );

    // ========= ⭐【关键1】全局检测（Arrow / Trap / Door） =========
    QList<QGraphicsItem*> nearby =
        scene()->items(selfRect.adjusted(-5, -5, 5, 5),
                       Qt::IntersectsItemBoundingRect);

    bool touchingTrap = false;

    for (QGraphicsItem* item : nearby) {
        QString type = item->data(0).toString();
        QRectF itemRect = item->sceneBoundingRect();

        // ✅ Arrow —— 用整体检测
        QRectF hitBox = selfRect.adjusted(5, 5, -5, -5);

        if (type == "Arrow" && hitBox.intersects(itemRect)) {
            isDead = true;
            if (deadHandler) deadHandler();
            return;
        }

        // ✅ Door —— 脚底
        if (type == "Door" && feetRect.intersects(itemRect)) {
            isDead = true;
            if (reachedDoorHandler) reachedDoorHandler();
            return;
        }

        // ✅ Trap —— 脚底
        if (type == "Trap" && feetRect.intersects(itemRect)) {
            touchingTrap = true;

            if (!trapTriggered && arrowRequestHandler) {
                arrowRequestHandler(this->y() + 10);
                trapTriggered = true;
            }
        }
    }

    // ========= ⭐【关键2】地面碰撞 =========
    bool foundGround = false;

    for (QGraphicsItem* item : collidingItems()) {
        if (!isSolidSurface(item)) continue;

        QRectF itemRect = item->sceneBoundingRect();

        if (yVelocity > 0 && jumpIgnoreTimer <= 0) {
            qreal overlap =
                qMin(selfRect.right(), itemRect.right()) -
                qMax(selfRect.left(), itemRect.left());

            qreal bottom = selfRect.bottom();
            qreal top = itemRect.top();
            qreal prevBottom = bottom - yVelocity;

            if (overlap > selfRect.width() * 0.5 &&
                prevBottom <= top + 5 &&
                bottom >= top - 5)
            {
                yVelocity = 0;
                isGrounded = true;
                foundGround = true;
                coyoteTimer = 10;

                setPos(x(), top - boundingRect().height());
                break;
            }
        }
    }

    // ========= 地面补偿 =========
    if (!foundGround) {
        QGraphicsItem* support =
            (jumpIgnoreTimer <= 0) ? findSupportingSurface() : nullptr;

        if (support) {
            qreal top = support->sceneBoundingRect().top();

            yVelocity = 0;
            isGrounded = true;
            foundGround = true;
            coyoteTimer = 10;

            setPos(x(), top - boundingRect().height());
        } else {
            if (isGrounded) coyoteTimer = 10;
            isGrounded = false;
        }

        if (y() > 600) {
            isDead = true;
            if (deadHandler) deadHandler();
            return;
        }
    }

    // ========= 跳跃缓冲 =========
    if (jumpRequested && jumpBufferTimer > 0 &&
        (isGrounded || coyoteTimer > 0)) {

        yVelocity = jumpForce;
        isGrounded = false;
        jumpIgnoreTimer = 6;

        jumpBufferTimer = 0;
        jumpRequested = false;
        coyoteTimer = 0;
    }

    updateAnimation();

    if (!touchingTrap) {
        trapTriggered = false;
    }
}

void Player::respawn(qreal x, qreal y) {
    isDead = false;
    isGrounded = false;
    leftPressed = false;
    rightPressed = false;
    trapTriggered = false;
    jumpRequested = false;
    xVelocity = 0;
    yVelocity = 0;
    currentFrame = 0;
    frameCounter = 0;
    jumpIgnoreTimer = 0;
    jumpBufferTimer = 0;
    coyoteTimer = 0;

    setPos(x, y);
    currentFrame = 0;
    jumpFrameIndex = 0;
    frameCounter = 0;
    animationState = IdleState;
    setPixmap(idlePixmap);
    setTransform(QTransform());
}

void Player::forceKill() {
    if (!isDead) {
        isDead = true;
        if (deadHandler) deadHandler();
    }
}

QRectF Player::boundingRect() const {
    QRectF rect = QGraphicsPixmapItem::boundingRect();

    // ✂️ 裁掉底部透明区域（根据你图片调整）
    rect.adjust(0, 0, 0, -8);

    return rect;
}

