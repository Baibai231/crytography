#include "VigenerePuzzle.h"
#include "uimanager.h"
#include "aimanager.h"
#include "aichatdialog.h"
#include "VigenereVisualizer.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QPropertyAnimation>

VigenerePuzzle::VigenerePuzzle(AIManager *manager, QWidget *parent)
    : QDialog(parent), aiManager(manager), m_stack(nullptr) {

    setWindowTitle("第三关：镜像祭坛");
    setFixedSize(560, 620);
    setStyleSheet(R"(
        QDialog { background-color: #151920; }
        QLabel { color: #f0e8d8; font-family: 'Microsoft YaHei'; }
        QLabel[role="title"] { font-size: 24px; font-weight: 700; color: #e4b45f; }
        QLabel[role="subtitle"] { font-size: 14px; color: #e2d5bd; }
        QLabel[role="hint"] { font-size: 15px; color: #eadfca; }
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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 24);
    mainLayout->setSpacing(18);

    QLabel *title = new QLabel("第三关：镜像祭坛", this);
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    QLabel *subtitle = new QLabel("对称的密钥，铸成锁与钥。完成加密与解密，方可通行。", this);
    subtitle->setProperty("role", "subtitle");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    mainLayout->addWidget(subtitle);

    m_stack = new QStackedWidget(this);

    setupPhase1();
    setupPhase2();

    mainLayout->addWidget(m_stack);

    // Bottom Hint Button
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(12);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *hintBtn = new QPushButton("询问神秘提示");
    hintBtn->setProperty("variant", "secondary");
    hintBtn->setFixedHeight(40);
    hintBtn->setCursor(Qt::PointingHandCursor);
    connect(hintBtn, &QPushButton::clicked, this, &VigenerePuzzle::requestHint);
    bottomLayout->addWidget(hintBtn);

    mainLayout->addLayout(bottomLayout);
    m_stack->setCurrentIndex(0);
}

void VigenerePuzzle::setupPhase1() {
    m_phase1Plain = "TEMPLE";
    m_phase1Key = "MIRROR";
    m_phase1Answer = "";
    for (int i = 0; i < m_phase1Plain.length(); ++i) {
        int p = m_phase1Plain[i].toLatin1() - 'A';
        int k = m_phase1Key[i % m_phase1Key.length()].toLatin1() - 'A';
        m_phase1Answer += QChar('A' + (p + k) % 26);
    }

    QWidget *phase1 = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(phase1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    QFrame *panel = new QFrame();
    panel->setProperty("role", "panel");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(12);

    QLabel *phaseLabel = new QLabel("【Phase 1】以钥铸锁");
    phaseLabel->setProperty("role", "subtitle");
    phaseLabel->setAlignment(Qt::AlignCenter);
    phaseLabel->setStyleSheet("font-weight: bold;");

    QLabel *info = new QLabel(QString("明文: <span style='color:#8ac4ff;'>%1</span> | 密钥: <span style='color:#66e078;'>%2</span>")
                                  .arg(m_phase1Plain).arg(m_phase1Key), panel);
    info->setProperty("role", "hint");
    info->setAlignment(Qt::AlignCenter);
    info->setWordWrap(true);

    panelLayout->addWidget(phaseLabel);
    panelLayout->addWidget(info);
    layout->addWidget(panel);

    m_inputEdit1 = new QLineEdit();
    m_inputEdit1->setPlaceholderText("请输入加密后的密文...");
    m_inputEdit1->setAlignment(Qt::AlignCenter);
    m_inputEdit1->setFixedHeight(45);
    layout->addWidget(m_inputEdit1);

    // Visualizer Button
    QPushButton *visualBtn = new QPushButton("进入加密推演可视台");
    visualBtn->setProperty("variant", "secondary");
    visualBtn->setFixedHeight(40);
    visualBtn->setCursor(Qt::PointingHandCursor);
    connect(visualBtn, &QPushButton::clicked, this, [=]() {
        showVisualizer(VigenereVisualizer::Encrypt, m_phase1Plain, m_phase1Key);
    });
    layout->addWidget(visualBtn);

    QPushButton *btn = new QPushButton("激活外门");
    btn->setFixedHeight(42);
    btn->setCursor(Qt::PointingHandCursor);
    connect(btn, &QPushButton::clicked, this, &VigenerePuzzle::checkPhase1);
    connect(m_inputEdit1, &QLineEdit::returnPressed, this, &VigenerePuzzle::checkPhase1);
    layout->addWidget(btn);

    m_stack->addWidget(phase1);
}

void VigenerePuzzle::setupPhase2() {
    m_phase2Key = "MAYA";
    m_phase2Answer = "EXIT";
    m_phase2Cipher = "";
    for (int i = 0; i < m_phase2Answer.length(); ++i) {
        int p = m_phase2Answer[i].toLatin1() - 'A';
        int k = m_phase2Key[i % m_phase2Key.length()].toLatin1() - 'A';
        m_phase2Cipher += QChar('A' + (p + k) % 26);
    }

    QWidget *phase2 = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(phase2);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(16);

    QFrame *panel = new QFrame();
    panel->setProperty("role", "panel");
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 18, 20, 18);
    panelLayout->setSpacing(12);

    QLabel *phaseLabel = new QLabel("【Phase 2】以钥启明");
    phaseLabel->setProperty("role", "subtitle");
    phaseLabel->setAlignment(Qt::AlignCenter);
    phaseLabel->setStyleSheet("font-weight: bold;");

    QLabel *info = new QLabel(QString("密文: <span style='color:#ffd58a;'>%1</span> | 密钥: <span style='color:#66e078;'>%2</span>")
                                  .arg(m_phase2Cipher).arg(m_phase2Key), panel);
    info->setProperty("role", "hint");
    info->setAlignment(Qt::AlignCenter);
    info->setWordWrap(true);

    panelLayout->addWidget(phaseLabel);
    panelLayout->addWidget(info);
    layout->addWidget(panel);

    m_inputEdit2 = new QLineEdit();
    m_inputEdit2->setPlaceholderText("请输入解密后的明文指令...");
    m_inputEdit2->setAlignment(Qt::AlignCenter);
    m_inputEdit2->setFixedHeight(45);
    layout->addWidget(m_inputEdit2);

    QPushButton *visualBtn2 = new QPushButton("进入解密推演可视台");
    visualBtn2->setProperty("variant", "secondary");
    visualBtn2->setFixedHeight(40);
    visualBtn2->setCursor(Qt::PointingHandCursor);
    connect(visualBtn2, &QPushButton::clicked, this, [=]() {
        showVisualizer(VigenereVisualizer::Decrypt, m_phase2Cipher, m_phase2Key);
    });
    layout->addWidget(visualBtn2);

    QPushButton *btn2 = new QPushButton("开启内殿");
    btn2->setFixedHeight(42);
    btn2->setCursor(Qt::PointingHandCursor);
    connect(btn2, &QPushButton::clicked, this, &VigenerePuzzle::checkPhase2);
    connect(m_inputEdit2, &QLineEdit::returnPressed, this, &VigenerePuzzle::checkPhase2);
    layout->addWidget(btn2);

    m_stack->addWidget(phase2);
}

void VigenerePuzzle::checkPhase1() {
    if (m_inputEdit1->text().trimmed().toUpper() == m_phase1Answer) {
        UIManager::showInfoDialog("第一阶段完成", "外门已激活，准备迎接第二阶段！", this);
        m_stack->setCurrentIndex(1);
    } else {
        shakeWindow();
        UIManager::showErrorDialog("失败", "加密错误，请检查密钥循环位移。", this);
    }
}

void VigenerePuzzle::checkPhase2() {
    if (m_inputEdit2->text().trimmed().toUpper() == m_phase2Answer) {
        UIManager::showInfoDialog("通关成功", "祭坛的双镜合二为一，通道已然显现！", this);
        accept();
    } else {
        shakeWindow();
        UIManager::showErrorDialog("失败", "咒语错误，祭坛光芒黯淡了下去。", this);
    }
}

void VigenerePuzzle::requestHint() {
    AIChatDialog dialog(aiManager, this);
    GameState state;
    state.puzzleType = "vigenere";
    state.encryptedText = m_phase1Answer;
    state.userInput = "";
    state.attemptCount = 0;
    state.solved = false;
    dialog.setGameState(state);
    dialog.exec();
}

void VigenerePuzzle::showVisualizer(VigenereVisualizer::Mode mode, const QString &input, const QString &key) {
    VigenereVisualizer viz(mode, input, key, this);
    viz.exec();
}

void VigenerePuzzle::shakeWindow() {
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
