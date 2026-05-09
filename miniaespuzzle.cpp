#include "MiniAESPuzzle.h"
#include "UIManager.h"
#include "aichatdialog.h"
#include "MiniAESVisualizer.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QGridLayout>

static void copyState(int dst[2][2], int src[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            dst[i][j] = src[i][j];
}

MiniAESPuzzle::MiniAESPuzzle(AIManager *manager, QWidget *parent)
    : QDialog(parent), aiManager(manager), m_stack(nullptr) {

    setWindowTitle("第四关：矩阵之钥");
    setFixedSize(580, 580);
    setModal(true);
    setStyleSheet(R"(
        QDialog { background-color: #151920; }
        QLabel { color: #f0e8d8; font-family: 'Microsoft YaHei'; }
        QLabel[role="title"] { font-size: 24px; font-weight: 700; color: #e4b45f; }
        QLabel[role="subtitle"] { font-size: 14px; color: #e2d5bd; }
        QLabel[role="hint"] { font-size: 15px; color: #eadfca; }
        QLabel[role="matrix-label"] {
            background-color: #1a1e26; border: 2px solid #3a3530; border-radius: 6px;
            color: #ffd58a; font-size: 22px; font-weight: 700;
            font-family: Consolas, 'Courier New', monospace;
        }
        QFrame[role="panel"] {
            background-color: rgba(44, 31, 22, 170); border: 1px solid rgba(228, 180, 95, 95);
            border-radius: 14px;
        }
        QLineEdit {
            padding: 10px 12px; font-size: 16px; border: 2px solid #6b5638; border-radius: 8px;
            background-color: #f8f3e8; color: #231912; letter-spacing: 2px;
        }
        QLineEdit:focus { border-color: #e4b45f; }
        QPushButton {
            background-color: #d7a84e; color: #1f160e; font-weight: 700; min-height: 44px;
            border-radius: 8px; font-size: 16px; border: none;
        }
        QPushButton:hover { background-color: #ecc166; }
        QPushButton:pressed { background-color: #c8973f; }
        QPushButton[variant="secondary"] {
            background-color: rgba(78, 58, 40, 215); color: #f7eedb;
            border: 1px solid rgba(234, 197, 141, 90);
        }
        QPushButton[variant="secondary"]:hover { background-color: rgba(101, 76, 53, 225); }
    )");

    generatePuzzle();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("第四关：矩阵之钥", this);
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("破解 Mini-AES 对称加密矩阵，找到失落的密钥", this);
    subtitle->setProperty("role", "subtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    mainLayout->addWidget(subtitle);

    m_stack = new QStackedWidget(this);

    setupPhase1();
    setupPhase2();
    setupPhase3();

    mainLayout->addWidget(m_stack);

    // Bottom buttons (shared)
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *hintBtn = new QPushButton("询问神秘提示");
    hintBtn->setProperty("variant", "secondary");
    hintBtn->setFixedHeight(40);
    hintBtn->setCursor(Qt::PointingHandCursor);
    connect(hintBtn, &QPushButton::clicked, this, &MiniAESPuzzle::requestHint);

    QPushButton *visualBtn = new QPushButton("打开矩阵推演台");
    visualBtn->setProperty("variant", "secondary");
    visualBtn->setFixedHeight(40);
    visualBtn->setCursor(Qt::PointingHandCursor);
    connect(visualBtn, &QPushButton::clicked, this, &MiniAESPuzzle::showVisualizer);

    bottomLayout->addWidget(hintBtn);
    bottomLayout->addWidget(visualBtn);
    mainLayout->addLayout(bottomLayout);

    m_stack->setCurrentIndex(0);
}

void MiniAESPuzzle::generatePuzzle() {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            m_plainText[i][j] = QRandomGenerator::global()->bounded(256);
            m_key[i][j] = QRandomGenerator::global()->bounded(256);
        }

    MiniAESVisualizer::encrypt(m_plainText, m_key, m_cipherText);
    m_steps = MiniAESVisualizer::generateSteps(m_plainText, m_key);

    m_phase1Expected = QString("%1%2%3%4")
        .arg(MiniAESVisualizer::toHex(m_cipherText[0][0]))
        .arg(MiniAESVisualizer::toHex(m_cipherText[0][1]))
        .arg(MiniAESVisualizer::toHex(m_cipherText[1][0]))
        .arg(MiniAESVisualizer::toHex(m_cipherText[1][1]));

    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++) {
            m_phase2Plain[i][j] = QRandomGenerator::global()->bounded(256);
            m_phase2Key[i][j] = QRandomGenerator::global()->bounded(256);
        }
    int cipher2[2][2];
    MiniAESVisualizer::encrypt(m_phase2Plain, m_phase2Key, cipher2);

    m_phase2Expected = QString("%1%2%3%4")
        .arg(MiniAESVisualizer::toHex(cipher2[0][0]))
        .arg(MiniAESVisualizer::toHex(cipher2[0][1]))
        .arg(MiniAESVisualizer::toHex(cipher2[1][0]))
        .arg(MiniAESVisualizer::toHex(cipher2[1][1]));

    int key3[2][2];
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            key3[i][j] = QRandomGenerator::global()->bounded(256);

    int plain3[2][2];
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            plain3[i][j] = QRandomGenerator::global()->bounded(256);

    MiniAESVisualizer::encrypt(plain3, key3, m_phase3Cipher);

    copyState(m_phase3Key, key3);

    m_phase3Expected = QString("%1%2%3%4")
        .arg(MiniAESVisualizer::toHex(plain3[0][0]))
        .arg(MiniAESVisualizer::toHex(plain3[0][1]))
        .arg(MiniAESVisualizer::toHex(plain3[1][0]))
        .arg(MiniAESVisualizer::toHex(plain3[1][1]));
}

void MiniAESPuzzle::advanceToPhase(int phase) {
    UIManager::showInfoDialog(
        phase == 1 ? "第二阶段解锁" : "最终阶段解锁",
        phase == 1 ? "你已理解 Mini-AES 的流程，现在亲手加密一组新数据！"
                    : "最后一道锁——已知密文和密钥，逆向还原明文！",
        this
    );
    m_stack->setCurrentIndex(phase);
}

QWidget* MiniAESPuzzle::createMatrixWidget(const QString &label, int data[2][2], const QString &accentColor) {
    QWidget *widget = new QWidget();
    QVBoxLayout *vLayout = new QVBoxLayout(widget);
    vLayout->setContentsMargins(0, 8, 0, 8);
    vLayout->setSpacing(6);

    QLabel *titleLabel = new QLabel(label);
    titleLabel->setStyleSheet(QString("color: %1; font-size: 14px; font-weight: bold;").arg(accentColor));
    titleLabel->setAlignment(Qt::AlignCenter);
    vLayout->addWidget(titleLabel);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            QLabel *cell = new QLabel(MiniAESVisualizer::toHex(data[i][j]));
            cell->setAlignment(Qt::AlignCenter);
            cell->setFixedSize(60, 56);
            cell->setStyleSheet(QString(
                "background-color: #1a1e26; border: 2px solid %1; border-radius: 6px;"
                "color: %2; font-size: 18px; font-weight: 700;"
                "font-family: Consolas, 'Courier New', monospace;"
            ).arg(accentColor).arg(accentColor));
            gridLayout->addWidget(cell, i, j);
        }
    }

    vLayout->addLayout(gridLayout);
    return widget;
}

void MiniAESPuzzle::setupPhase1() {
    QWidget *phase1 = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(phase1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    QFrame *panel = new QFrame();
    panel->setProperty("role", "panel");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(16);

    QLabel *phaseLabel = new QLabel("【Phase 1】观察加密流程");
    phaseLabel->setProperty("role", "subtitle");
    phaseLabel->setAlignment(Qt::AlignCenter);
    phaseLabel->setStyleSheet("font-weight: bold;");
    panelLayout->addWidget(phaseLabel);

    // Matrices row
    QHBoxLayout *matricesRow = new QHBoxLayout();
    matricesRow->setSpacing(20);
    matricesRow->addStretch();
    matricesRow->addWidget(createMatrixWidget("明文矩阵", m_plainText, "#8ac4ff"));
    matricesRow->addWidget(createMatrixWidget("密钥矩阵", m_key, "#66e078"));
    matricesRow->addStretch();
    panelLayout->addLayout(matricesRow);

    layout->addWidget(panel);

    QLabel *instruction = new QLabel("点击按钮打开可视化推演台，观察加密的每一步", this);
    instruction->setProperty("role", "hint");
    instruction->setAlignment(Qt::AlignCenter);
    instruction->setWordWrap(true);
    layout->addWidget(instruction);

    m_inputEdit1 = new QLineEdit();
    m_inputEdit1->setPlaceholderText("观察后输入最终密文（8位十六进制，如 3A1F7C9B）...");
    m_inputEdit1->setAlignment(Qt::AlignCenter);
    m_inputEdit1->setFixedHeight(45);
    layout->addWidget(m_inputEdit1);

    QPushButton *btn = new QPushButton("激活外门");
    btn->setFixedHeight(42);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &MiniAESPuzzle::checkPhase1);
    connect(m_inputEdit1, &QLineEdit::returnPressed, this, &MiniAESPuzzle::checkPhase1);
    layout->addWidget(btn);

    m_stack->addWidget(phase1);
}

void MiniAESPuzzle::setupPhase2() {
    QWidget *phase2 = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(phase2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    QFrame *panel = new QFrame();
    panel->setProperty("role", "panel");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(16);

    QLabel *phaseLabel = new QLabel("【Phase 2】亲手加密");
    phaseLabel->setProperty("role", "subtitle");
    phaseLabel->setAlignment(Qt::AlignCenter);
    phaseLabel->setStyleSheet("font-weight: bold;");
    panelLayout->addWidget(phaseLabel);

    // Matrices row
    QHBoxLayout *matricesRow = new QHBoxLayout();
    matricesRow->setSpacing(20);
    matricesRow->addStretch();
    matricesRow->addWidget(createMatrixWidget("明文矩阵", m_phase2Plain, "#8ac4ff"));
    matricesRow->addWidget(createMatrixWidget("密钥矩阵", m_phase2Key, "#66e078"));
    matricesRow->addStretch();
    panelLayout->addLayout(matricesRow);

    layout->addWidget(panel);

    m_inputEdit2 = new QLineEdit();
    m_inputEdit2->setPlaceholderText("请输入加密后的密文（8位十六进制）...");
    m_inputEdit2->setAlignment(Qt::AlignCenter);
    m_inputEdit2->setFixedHeight(45);
    layout->addWidget(m_inputEdit2);

    QPushButton *btn = new QPushButton("激活内门");
    btn->setFixedHeight(42);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &MiniAESPuzzle::checkPhase2);
    connect(m_inputEdit2, &QLineEdit::returnPressed, this, &MiniAESPuzzle::checkPhase2);
    layout->addWidget(btn);

    m_stack->addWidget(phase2);
}

void MiniAESPuzzle::setupPhase3() {
    QWidget *phase3 = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(phase3);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(14);

    QFrame *panel = new QFrame();
    panel->setProperty("role", "panel");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(16);

    QLabel *phaseLabel = new QLabel("【Phase 3】逆向解密");
    phaseLabel->setProperty("role", "subtitle");
    phaseLabel->setAlignment(Qt::AlignCenter);
    phaseLabel->setStyleSheet("font-weight: bold;");
    panelLayout->addWidget(phaseLabel);

    // Matrices row
    QHBoxLayout *matricesRow = new QHBoxLayout();
    matricesRow->setSpacing(20);
    matricesRow->addStretch();
    matricesRow->addWidget(createMatrixWidget("密文矩阵", m_phase3Cipher, "#ffd58a"));
    matricesRow->addWidget(createMatrixWidget("密钥矩阵", m_phase3Key, "#66e078"));
    matricesRow->addStretch();
    panelLayout->addLayout(matricesRow);

    layout->addWidget(panel);

    m_inputEdit3 = new QLineEdit();
    m_inputEdit3->setPlaceholderText("请输入解密后的明文（8位十六进制）...");
    m_inputEdit3->setAlignment(Qt::AlignCenter);
    m_inputEdit3->setFixedHeight(45);
    layout->addWidget(m_inputEdit3);

    QPushButton *btn = new QPushButton("破解矩阵之钥");
    btn->setFixedHeight(42);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &MiniAESPuzzle::checkPhase3);
    connect(m_inputEdit3, &QLineEdit::returnPressed, this, &MiniAESPuzzle::checkPhase3);
    layout->addWidget(btn);

    m_stack->addWidget(phase3);
}

void MiniAESPuzzle::checkPhase1() {
    QString input = m_inputEdit1->text().trimmed().toUpper();
    if (input == m_phase1Expected) {
        UIManager::showInfoDialog("第一阶段完成", "你成功观察并验证了加密流程！", this);
        advanceToPhase(1);
    } else {
        shakeWindow();
        UIManager::showErrorDialog("失败", "密文不正确，请打开推演台重新观察。", this);
    }
}

void MiniAESPuzzle::checkPhase2() {
    QString input = m_inputEdit2->text().trimmed().toUpper();
    if (input == m_phase2Expected) {
        UIManager::showInfoDialog("第二阶段完成", "加密正确！你已掌握 Mini-AES！", this);
        advanceToPhase(2);
    } else {
        shakeWindow();
        UIManager::showErrorDialog("失败", "加密结果错误，请检查每一步运算。", this);
    }
}

void MiniAESPuzzle::checkPhase3() {
    QString input = m_inputEdit3->text().trimmed().toUpper();
    if (input == m_phase3Expected) {
        UIManager::showInfoDialog("通关成功", "矩阵之钥已被破解，通道开启！", this);
        accept();
    } else {
        shakeWindow();
        UIManager::showErrorDialog("失败", "解密错误，矩阵重新锁定了。", this);
    }
}

void MiniAESPuzzle::showVisualizer() {
    MiniAESVisualizer viz(m_plainText, m_key, MiniAESVisualizer::Demo, this);
    viz.exec();
}

void MiniAESPuzzle::requestHint() {
    AIChatDialog dialog(aiManager, this);
    GameState state;
    state.puzzleType = "mini_aes";
    state.encryptedText = m_phase1Expected;
    state.userInput = "";
    state.attemptCount = 0;
    state.solved = false;
    dialog.setGameState(state);
    dialog.exec();
}

void MiniAESPuzzle::shakeWindow() {
    QPoint originalPos = this->pos();
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(300);
    animation->setKeyValueAt(0, originalPos);
    animation->setKeyValueAt(0.1, originalPos + QPoint(-10, 0));
    animation->setKeyValueAt(0.2, originalPos + QPoint(10, 0));
    animation->setKeyValueAt(0.3, originalPos + QPoint(-10, 0));
    animation->setKeyValueAt(0.4, originalPos + QPoint(10, 0));
    animation->setKeyValueAt(0.5, originalPos);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
