#include "LevelSelectDialog.h"
#include "UIManager.h"
#include "GameProgress.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QStyle>
#include <QGraphicsDropShadowEffect>

// ================= 构造 =================
LevelSelectDialog::LevelSelectDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("选择关卡");
    setFixedSize(600, 500);
    setModal(true);

    setStyleSheet(R"(
        QDialog {
            background-color: #151920;
            border: 1px solid #e4b45f;
            border-radius: 14px;
        }
    )");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ===== Header =====
    QWidget *header = new QWidget(this);
    header->setFixedHeight(60);

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel *titleLabel = new QLabel("关卡选择", header);
    titleLabel->setStyleSheet("color: #e4b45f; font-size: 24px; font-weight: bold;");

    m_resetBtn = new QPushButton("重置进度", header);
    m_resetBtn->setCursor(Qt::PointingHandCursor);
    m_resetBtn->setStyleSheet(R"(
        QPushButton {
            background: transparent;
            color: #888;
            border: 1px solid #555;
            border-radius: 4px;
            padding: 4px 8px;
        }
        QPushButton:hover {
            color: #c96b56;
            border-color: #c96b56;
        }
    )");

    connect(m_resetBtn, &QPushButton::clicked, this, &LevelSelectDialog::resetProgress);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_resetBtn);

    mainLayout->addWidget(header);

    // ===== 地图区域 =====
    m_mapArea = new QWidget(this);
    m_mapArea->setMinimumHeight(400);
    m_mapArea->setStyleSheet("background-color: transparent;");
    mainLayout->addWidget(m_mapArea);

    const int btnSize = 50;

    // ===== 关卡按钮 =====
    m_lvl1Btn = new QPushButton("1", m_mapArea);
    m_lvl1Btn->setFixedSize(btnSize, btnSize);
    m_lvl1Btn->move(280, 300);

    m_lvl2Btn = new QPushButton("2", m_mapArea);
    m_lvl2Btn->setFixedSize(btnSize, btnSize);
    m_lvl2Btn->move(250, 200);

    m_lvl3Btn = new QPushButton("3", m_mapArea);
    m_lvl3Btn->setFixedSize(btnSize, btnSize);
    m_lvl3Btn->move(300, 110);

    m_lvl4Btn = new QPushButton("4", m_mapArea);
    m_lvl4Btn->setFixedSize(btnSize, btnSize);
    m_lvl4Btn->move(340, 30);

    // ===== 圆形样式 =====
    QString btnStyle = R"(
        QPushButton {
            border-radius: 25px;
            background-color: #3a3a3a;
            color: #777;
            font-size: 18px;
            font-weight: bold;
            border: 2px solid #2a2a2a;
        }

        QPushButton[status="unlocked"] {
            background-color: #e4b45f;
            color: #1f160e;
            border: 2px solid #c8973f;
        }

        QPushButton[status="unlocked"]:hover {
            background-color: #f0c57a;
        }
    )";

    m_lvl1Btn->setStyleSheet(btnStyle);
    m_lvl2Btn->setStyleSheet(btnStyle);
    m_lvl3Btn->setStyleSheet(btnStyle);
    m_lvl4Btn->setStyleSheet(btnStyle);

    m_lvl1Btn->setCursor(Qt::PointingHandCursor);
    m_lvl2Btn->setCursor(Qt::PointingHandCursor);
    m_lvl3Btn->setCursor(Qt::PointingHandCursor);
    m_lvl4Btn->setCursor(Qt::PointingHandCursor);

    // ===== 信号 =====
    connect(m_lvl1Btn, &QPushButton::clicked, this, [this]() { handleLevelClick(1); });
    connect(m_lvl2Btn, &QPushButton::clicked, this, [this]() { handleLevelClick(2); });
    connect(m_lvl3Btn, &QPushButton::clicked, this, [this]() { handleLevelClick(3); });
    connect(m_lvl4Btn, &QPushButton::clicked, this, [this]() { handleLevelClick(4); });

    updateButtonStyles();
}

// ================= 绘制路径 =================
void LevelSelectDialog::paintEvent(QPaintEvent *event) {
    QDialog::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPoint p1 = m_lvl1Btn->mapTo(this, QPoint(25, 25));
    QPoint p2 = m_lvl2Btn->mapTo(this, QPoint(25, 25));
    QPoint p3 = m_lvl3Btn->mapTo(this, QPoint(25, 25));
    QPoint p4 = m_lvl4Btn->mapTo(this, QPoint(25, 25));

    // ===== 细白路径 =====
    QPen pen(QColor(255, 255, 255, 180), 2);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    QPainterPath path12(p1);
    path12.cubicTo(
        p1 + QPoint(-30, -60),
        p2 + QPoint(30, 60),
        p2
        );
    painter.drawPath(path12);

    QPainterPath path23(p2);
    path23.cubicTo(
        p2 + QPoint(-30, -60),
        p3 + QPoint(30, 60),
        p3
        );
    painter.drawPath(path23);

    QPainterPath path34(p3);
    path34.cubicTo(
        p3 + QPoint(40, -20),
        p4 + QPoint(-40, 30),
        p4
        );
    painter.drawPath(path34);
}

// ================= 状态更新 =================
void LevelSelectDialog::updateButtonStyles() {
    int maxUnlocked = GameProgress::getMaxUnlockedLevel();

    auto setStatus = [](QPushButton *btn, bool unlocked) {
        btn->setProperty("status", unlocked ? "unlocked" : "locked");

        btn->style()->unpolish(btn);
        btn->style()->polish(btn);
        btn->update();

        btn->setGraphicsEffect(nullptr);

        // ===== 轻微发光 =====
        if (unlocked) {
            auto *shadow = new QGraphicsDropShadowEffect(btn);
            shadow->setBlurRadius(12);
            shadow->setOffset(0);
            shadow->setColor(QColor(228, 180, 95, 120));
            btn->setGraphicsEffect(shadow);
        }
    };

    setStatus(m_lvl1Btn, maxUnlocked >= 1);
    setStatus(m_lvl2Btn, maxUnlocked >= 2);
    setStatus(m_lvl3Btn, maxUnlocked >= 3);
    setStatus(m_lvl4Btn, maxUnlocked >= 4);
}

// ================= 点击逻辑 =================
void LevelSelectDialog::handleLevelClick(int level) {
    int maxUnlocked = GameProgress::getMaxUnlockedLevel();

    if (level <= maxUnlocked) {
        emit levelSelected(level);
        accept();
    } else {
        UIManager::showErrorDialog("未解锁", "请先通关前置关卡！", this);
    }
}

// ================= 重置进度 =================
void LevelSelectDialog::resetProgress() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("重置进度");
    msgBox.setText("确定要清空所有关卡进度吗？");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    msgBox.setStyleSheet(R"(
        QLabel {
            font-size: 12px;
        }
        QPushButton {
            font-size: 12px;
            padding: 4px 10px;
        }
    )");

    if (msgBox.exec() == QMessageBox::Yes) {
        GameProgress::reset();
        updateButtonStyles();
    }
}
