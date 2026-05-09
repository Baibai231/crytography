#include "miniaesvisualizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPropertyAnimation>
#include <QTimer>
#include <QStyle>

const int MiniAESVisualizer::SBOX[256] = {
    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
};

static const int RCON[3][2][2] = {
    {{0x00, 0x00}, {0x00, 0x00}},
    {{0x01, 0x00}, {0x00, 0x00}},
    {{0x02, 0x00}, {0x00, 0x00}}
};

QString MiniAESVisualizer::toHex(int v) {
    return QString("%1").arg(v & 0xFF, 2, 16, QChar('0')).toUpper();
}

static void copyState(int dst[2][2], const int src[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            dst[i][j] = src[i][j];
}

static void subBytes(int state[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            state[i][j] = MiniAESVisualizer::SBOX[state[i][j] & 0xFF];
}

static void shiftRows(int state[2][2]) {
    int tmp = state[1][0];
    state[1][0] = state[1][1];
    state[1][1] = tmp;
}

static void mixColumns(int state[2][2]) {
    int t0 = state[0][0] ^ state[1][0];
    int t1 = state[0][1] ^ state[1][1];
    state[1][0] = state[0][0];
    state[1][1] = state[0][1];
    state[0][0] = t0;
    state[0][1] = t1;
}

static void addRoundKey(int state[2][2], int key[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            state[i][j] = state[i][j] ^ key[i][j];
}

static void getRoundKey(int key[2][2], int rk[2][2], int round) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            rk[i][j] = key[i][j] ^ RCON[round][i][j];
}

void MiniAESVisualizer::encrypt(int plain[2][2], int key[2][2], int result[2][2]) {
    int state[2][2];
    copyState(state, plain);

    int rk[2][2];
    getRoundKey(key, rk, 0);
    addRoundKey(state, rk);

    getRoundKey(key, rk, 1);
    subBytes(state);
    shiftRows(state);
    mixColumns(state);
    addRoundKey(state, rk);

    getRoundKey(key, rk, 2);
    subBytes(state);
    shiftRows(state);
    addRoundKey(state, rk);

    copyState(result, state);
}

QList<AesStep> MiniAESVisualizer::generateSteps(int plain[2][2], int key[2][2]) {
    QList<AesStep> steps;
    int state[2][2];
    copyState(state, plain);
    int before[2][2];
    int rk[2][2];
    AesStep step;

    // Key Schedule (pre-step 0)
    copyState(before, state);
    step.name = "密钥编排 (Key Schedule)";
    step.detail = QString("原始密钥 K：\n"
        "[%1 %2]  [%3 %4]\n\n"
        "轮密钥生成规则：\n"
        "RoundKey[r][i][j] = Key[i][j] ⊕ RCON[r][i][j]\n\n"
        "RCON常量：\n"
        "  RCON[0] = 00  → RoundKey0 = K\n"
        "  RCON[1] = 01  → RoundKey1[0][0] = %1⊕01\n"
        "  RCON[2] = 02  → RoundKey2[0][0] = %1⊕02\n"
        "其余位置与 K 相同")
              .arg(toHex(key[0][0])).arg(toHex(key[0][1]))
              .arg(toHex(key[1][0])).arg(toHex(key[1][1]))
              .arg(toHex(key[0][0])).arg(toHex(key[0][0]));
    copyState(step.before, before);
    copyState(step.after, before);
    steps.append(step);

    // Round 0: AddRoundKey
    getRoundKey(key, rk, 0);
    copyState(before, state);
    step.name = "第 0 轮：轮密钥加";
    step.detail = QString("状态矩阵 ⊕ RoundKey0（原始密钥）\n")
        + QString("%1⊕%2=%3  %4⊕%5=%6\n")
              .arg(toHex(before[0][0])).arg(toHex(rk[0][0])).arg(toHex(before[0][0]^rk[0][0]))
              .arg(toHex(before[0][1])).arg(toHex(rk[0][1])).arg(toHex(before[0][1]^rk[0][1]))
        + QString("%1⊕%2=%3  %4⊕%5=%6")
              .arg(toHex(before[1][0])).arg(toHex(rk[1][0])).arg(toHex(before[1][0]^rk[1][0]))
              .arg(toHex(before[1][1])).arg(toHex(rk[1][1])).arg(toHex(before[1][1]^rk[1][1]));
    copyState(step.before, before);
    addRoundKey(state, rk);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: SubBytes
    getRoundKey(key, rk, 1);
    copyState(before, state);
    step.name = "第 1 轮：字节替换 SubBytes";
    step.detail = QString("AES S-Box 字节替换\n")
        + QString("SBox[%1]=%2  SBox[%3]=%4\n")
              .arg(toHex(before[0][0])).arg(toHex(SBOX[before[0][0] & 0xFF]))
              .arg(toHex(before[0][1])).arg(toHex(SBOX[before[0][1] & 0xFF]))
        + QString("SBox[%1]=%2  SBox[%3]=%4")
              .arg(toHex(before[1][0])).arg(toHex(SBOX[before[1][0] & 0xFF]))
              .arg(toHex(before[1][1])).arg(toHex(SBOX[before[1][1] & 0xFF]));
    copyState(step.before, before);
    subBytes(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: ShiftRows
    copyState(before, state);
    step.name = "第 1 轮：行移位 ShiftRows";
    step.detail = QString("第 1 行循环左移一位\n")
        + QString("[%1 %2] → [%3 %4]")
              .arg(toHex(before[1][0])).arg(toHex(before[1][1]))
              .arg(toHex(before[1][1])).arg(toHex(before[1][0]));
    copyState(step.before, before);
    shiftRows(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: MixColumns
    copyState(before, state);
    step.name = "第 1 轮：列混合 MixColumns";
    step.detail = QString("列混合（逐列异或运算）\n")
        + QString("%1⊕%2=%3  %4⊕%5=%6\n")
              .arg(toHex(before[0][0])).arg(toHex(before[1][0])).arg(toHex(before[0][0]^before[1][0]))
              .arg(toHex(before[0][1])).arg(toHex(before[1][1])).arg(toHex(before[0][1]^before[1][1]))
        + QString("原上半部分移至下半部分，异或结果放上半部分");
    copyState(step.before, before);
    mixColumns(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: AddRoundKey
    copyState(before, state);
    step.name = "第 1 轮：轮密钥加";
    step.detail = QString("状态矩阵 ⊕ RoundKey1\n"
        "RoundKey1[0][0] = 原始密钥[0][0] ⊕ 0x01\n")
        + QString("%1⊕%2=%3  %4⊕%5=%6\n")
              .arg(toHex(before[0][0])).arg(toHex(rk[0][0])).arg(toHex(before[0][0]^rk[0][0]))
              .arg(toHex(before[0][1])).arg(toHex(rk[0][1])).arg(toHex(before[0][1]^rk[0][1]))
        + QString("%1⊕%2=%3  %4⊕%5=%6")
              .arg(toHex(before[1][0])).arg(toHex(rk[1][0])).arg(toHex(before[1][0]^rk[1][0]))
              .arg(toHex(before[1][1])).arg(toHex(rk[1][1])).arg(toHex(before[1][1]^rk[1][1]));
    copyState(step.before, before);
    addRoundKey(state, rk);
    copyState(step.after, state);
    steps.append(step);

    // Round 2: SubBytes
    getRoundKey(key, rk, 2);
    copyState(before, state);
    step.name = "第 2 轮：字节替换 SubBytes";
    step.detail = QString("AES S-Box 字节替换\n")
        + QString("SBox[%1]=%2  SBox[%3]=%4\n")
              .arg(toHex(before[0][0])).arg(toHex(SBOX[before[0][0] & 0xFF]))
              .arg(toHex(before[0][1])).arg(toHex(SBOX[before[0][1] & 0xFF]))
        + QString("SBox[%1]=%2  SBox[%3]=%4")
              .arg(toHex(before[1][0])).arg(toHex(SBOX[before[1][0] & 0xFF]))
              .arg(toHex(before[1][1])).arg(toHex(SBOX[before[1][1] & 0xFF]));
    copyState(step.before, before);
    subBytes(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 2: ShiftRows
    copyState(before, state);
    step.name = "第 2 轮：行移位 ShiftRows";
    step.detail = QString("第 1 行循环左移一位\n")
        + QString("[%1 %2] → [%3 %4]")
              .arg(toHex(before[1][0])).arg(toHex(before[1][1]))
              .arg(toHex(before[1][1])).arg(toHex(before[1][0]));
    copyState(step.before, before);
    shiftRows(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 2: AddRoundKey
    copyState(before, state);
    step.name = "第 2 轮：轮密钥加（最终密文）";
    step.detail = QString("状态矩阵 ⊕ RoundKey2 —— 最终密文！\n"
        "RoundKey2[0][0] = 原始密钥[0][0] ⊕ 0x02\n")
        + QString("%1⊕%2=%3  %4⊕%5=%6\n")
              .arg(toHex(before[0][0])).arg(toHex(rk[0][0])).arg(toHex(before[0][0]^rk[0][0]))
              .arg(toHex(before[0][1])).arg(toHex(rk[0][1])).arg(toHex(before[0][1]^rk[0][1]))
        + QString("%1⊕%2=%3  %4⊕%5=%6")
              .arg(toHex(before[1][0])).arg(toHex(rk[1][0])).arg(toHex(before[1][0]^rk[1][0]))
              .arg(toHex(before[1][1])).arg(toHex(rk[1][1])).arg(toHex(before[1][1]^rk[1][1]));
    copyState(step.before, before);
    addRoundKey(state, rk);
    copyState(step.after, state);
    steps.append(step);

    return steps;
}

MiniAESVisualizer::MiniAESVisualizer(int plain[2][2], int key[2][2], Mode mode, QWidget *parent)
    : QDialog(parent), m_mode(mode) {
    copyState(m_state, plain);
    copyState(m_key, key);

    m_steps = generateSteps(plain, key);

    setWindowTitle("Mini-AES 矩阵推演 (AES S-Box)");
    setFixedSize(500, 600);
    setModal(true);
    setStyleSheet(R"(
        QDialog { background-color: #151920; }
        QLabel { color: #f0e8d8; font-family: 'Microsoft YaHei'; }
        QLabel[role="title"] { font-size: 20px; font-weight: 700; color: #e4b45f; }
        QLabel[role="step-name"] { font-size: 17px; font-weight: 700; color: #ffd58a; }
        QLabel[role="step-detail"] { font-size: 14px; color: #d7cfc0; font-family: Consolas, 'Courier New', monospace; }
        QLabel[role="matrix"] {
            background-color: #1a1e26; border: 2px solid #3a3530; border-radius: 6px;
            color: #ffd58a; font-size: 24px; font-weight: 700;
            font-family: Consolas, 'Courier New', monospace;
        }
        QLabel[role="matrix"][highlight="true"] {
            border: 2px solid #e4b45f; color: #ffe8a0;
            background-color: #2a2520;
        }
        QPushButton {
            background-color: #d7a84e; color: #1f160e; font-weight: 700;
            min-height: 36px; border-radius: 8px; font-size: 14px; border: none;
        }
        QPushButton:hover { background-color: #ecc166; }
        QPushButton:pressed { background-color: #c8973f; }
        QPushButton:disabled {
            background-color: rgba(78, 58, 40, 215); color: #8a7e6a;
            border: 1px solid rgba(234, 197, 141, 50);
        }
        QPushButton[variant="secondary"] {
            background-color: rgba(78, 58, 40, 215); color: #f7eedb;
            border: 1px solid rgba(234, 197, 141, 90);
        }
        QFrame[role="panel"] {
            background-color: rgba(44, 31, 22, 170); border: 1px solid rgba(228, 180, 95, 95);
            border-radius: 14px;
        }
        QLabel[role="counter"] { color: #8a7e6a; font-size: 13px; }
    )");

    setupUI();
    m_currentStep = -1;
    nextStep();
}

void MiniAESVisualizer::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 16, 20, 16);
    mainLayout->setSpacing(14);

    // Title
    QLabel *title = new QLabel("Mini-AES 加密推演", this);
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // State Matrix (centered)
    QVBoxLayout *matrixLayout = new QVBoxLayout();
    matrixLayout->setSpacing(10);
    matrixLayout->setAlignment(Qt::AlignCenter);

    QLabel *matrixTitle = new QLabel("状态矩阵 State Matrix", this);
    matrixTitle->setProperty("role", "subtitle");
    matrixTitle->setAlignment(Qt::AlignCenter);
    matrixLayout->addWidget(matrixTitle);

    QHBoxLayout *matrixRow = new QHBoxLayout();
    matrixRow->setAlignment(Qt::AlignCenter);
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(8);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            m_matrixLabels[i][j] = new QLabel("00", this);
            m_matrixLabels[i][j]->setProperty("role", "matrix");
            m_matrixLabels[i][j]->setFixedSize(72, 72);
            m_matrixLabels[i][j]->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(m_matrixLabels[i][j], i, j);
        }
    }
    matrixRow->addLayout(gridLayout);
    matrixLayout->addLayout(matrixRow);

    mainLayout->addLayout(matrixLayout);

    // Step display panel
    QFrame *stepPanel = new QFrame(this);
    stepPanel->setProperty("role", "panel");
    QVBoxLayout *stepLayout = new QVBoxLayout(stepPanel);
    stepLayout->setContentsMargins(16, 14, 16, 14);
    stepLayout->setSpacing(8);

    QHBoxLayout *stepHeaderLayout = new QHBoxLayout();
    m_stepNameLabel = new QLabel("—", stepPanel);
    m_stepNameLabel->setProperty("role", "step-name");
    m_stepCounterLabel = new QLabel("—", stepPanel);
    m_stepCounterLabel->setProperty("role", "counter");
    m_stepCounterLabel->setAlignment(Qt::AlignRight);
    stepHeaderLayout->addWidget(m_stepNameLabel);
    stepHeaderLayout->addWidget(m_stepCounterLabel);
    stepLayout->addLayout(stepHeaderLayout);

    m_stepDetailLabel = new QLabel("点击下一步开始推演", stepPanel);
    m_stepDetailLabel->setProperty("role", "step-detail");
    m_stepDetailLabel->setWordWrap(true);
    stepLayout->addWidget(m_stepDetailLabel);

    QHBoxLayout *sboxBtnRow = new QHBoxLayout();
    sboxBtnRow->setContentsMargins(0, 4, 0, 0);
    QPushButton *sboxBtn = new QPushButton("点击查看 S-Box 替换表", stepPanel);
    sboxBtn->setProperty("variant", "secondary");
    sboxBtn->setCursor(Qt::PointingHandCursor);
    sboxBtn->setFixedHeight(34);
    connect(sboxBtn, &QPushButton::clicked, this, &MiniAESVisualizer::showSBoxPopup);
    sboxBtnRow->addStretch();
    sboxBtnRow->addWidget(sboxBtn);
    sboxBtnRow->addStretch();
    stepLayout->addLayout(sboxBtnRow);

    mainLayout->addWidget(stepPanel);

    // Controls
    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    ctrlLayout->setSpacing(10);

    m_prevBtn = new QPushButton("← 上一步", this);
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    connect(m_prevBtn, &QPushButton::clicked, this, &MiniAESVisualizer::prevStep);

    m_nextBtn = new QPushButton("下一步 →", this);
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    connect(m_nextBtn, &QPushButton::clicked, this, &MiniAESVisualizer::nextStep);

    m_autoBtn = new QPushButton("▶ 自动播放", this);
    m_autoBtn->setProperty("variant", "secondary");
    m_autoBtn->setCursor(Qt::PointingHandCursor);
    connect(m_autoBtn, &QPushButton::clicked, this, &MiniAESVisualizer::toggleAutoPlay);

    ctrlLayout->addWidget(m_prevBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(m_autoBtn);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(m_nextBtn);
    mainLayout->addLayout(ctrlLayout);

    m_autoTimer = new QTimer(this);
    m_autoTimer->setInterval(2000);
    connect(m_autoTimer, &QTimer::timeout, this, [this]() {
        if (m_currentStep < m_steps.size() - 1) {
            nextStep();
        } else {
            toggleAutoPlay();
        }
    });
}

void MiniAESVisualizer::updateMatrixDisplay() {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            m_matrixLabels[i][j]->setText(toHex(m_state[i][j]));
            m_matrixLabels[i][j]->setProperty("highlight", false);
            m_matrixLabels[i][j]->style()->unpolish(m_matrixLabels[i][j]);
            m_matrixLabels[i][j]->style()->polish(m_matrixLabels[i][j]);
        }
    }
}

void MiniAESVisualizer::highlightCells(const int cells[4][2], int count) {
    for (int k = 0; k < count; k++) {
        int r = cells[k][0];
        int c = cells[k][1];
        if (r >= 0 && r < 2 && c >= 0 && c < 2) {
            m_matrixLabels[r][c]->setProperty("highlight", true);
            m_matrixLabels[r][c]->style()->unpolish(m_matrixLabels[r][c]);
            m_matrixLabels[r][c]->style()->polish(m_matrixLabels[r][c]);
        }
    }
}

void MiniAESVisualizer::updateStepDisplay() {
    if (m_currentStep < 0 || m_currentStep >= m_steps.size()) return;

    const AesStep &step = m_steps[m_currentStep];
    m_stepNameLabel->setText(step.name);
    m_stepDetailLabel->setText(step.detail);
    m_stepCounterLabel->setText(QString("%1 / %2").arg(m_currentStep + 1).arg(m_steps.size()));

    copyState(m_state, step.after);
    updateMatrixDisplay();

    static const int allCells[4][2] = {{0,0},{0,1},{1,0},{1,1}};
    highlightCells(allCells, 4);

    m_prevBtn->setEnabled(m_currentStep > 0);
    m_nextBtn->setEnabled(m_currentStep < m_steps.size() - 1);
}

void MiniAESVisualizer::prevStep() {
    if (m_currentStep <= 0) return;
    m_currentStep--;
    const AesStep &step = m_steps[m_currentStep];
    copyState(m_state, step.after);
    updateStepDisplay();
}

void MiniAESVisualizer::nextStep() {
    if (m_currentStep >= m_steps.size() - 1) return;
    m_currentStep++;
    updateStepDisplay();
}

void MiniAESVisualizer::showSBoxPopup() {
    QDialog *popup = new QDialog(this);
    popup->setWindowTitle("AES S-Box 查找表");
    popup->setFixedSize(420, 420);
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->setStyleSheet(R"(
        QDialog { background-color: #151920; }
        QLabel { color: #f0e8d8; font-family: 'Microsoft YaHei'; }
        QLabel[role="title"] { font-size: 16px; font-weight: 700; color: #e4b45f; }
        QLabel[role="sbox"] {
            background-color: #1a1e26; border: 1px solid #2a2520; border-radius: 2px;
            color: #a09880; font-size: 9px; text-align: center;
            font-family: Consolas, 'Courier New', monospace;
        }
        QLabel[role="sbox-header"] { color: #8a7e6a; font-size: 10px; }
    )");

    QVBoxLayout *layout = new QVBoxLayout(popup);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(6);

    QLabel *title = new QLabel("AES S-Box 查找表 (16×16)", popup);
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);

    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(1);
    grid->setAlignment(Qt::AlignCenter);

    QLabel *corner = new QLabel("x\\y", popup);
    corner->setProperty("role", "sbox-header");
    corner->setAlignment(Qt::AlignCenter);
    grid->addWidget(corner, 0, 0);

    for (int j = 0; j < 16; j++) {
        QLabel *h = new QLabel(QString("%1").arg(j, 1, 16, QChar('0')).toUpper(), popup);
        h->setProperty("role", "sbox-header");
        h->setAlignment(Qt::AlignCenter);
        h->setFixedSize(22, 18);
        grid->addWidget(h, 0, j + 1);
    }
    for (int i = 0; i < 16; i++) {
        QLabel *v = new QLabel(QString("%1x").arg(i, 1, 16, QChar('0')).toUpper(), popup);
        v->setProperty("role", "sbox-header");
        v->setAlignment(Qt::AlignCenter);
        v->setFixedSize(22, 18);
        grid->addWidget(v, i + 1, 0);
        for (int j = 0; j < 16; j++) {
            int val = SBOX[i * 16 + j];
            QLabel *cell = new QLabel(toHex(val), popup);
            cell->setProperty("role", "sbox");
            cell->setFixedSize(22, 18);
            cell->setAlignment(Qt::AlignCenter);
            grid->addWidget(cell, i + 1, j + 1);
        }
    }

    layout->addLayout(grid);
    popup->setModal(false);
    popup->show();
}

void MiniAESVisualizer::toggleAutoPlay() {
    if (m_autoPlaying) {
        m_autoTimer->stop();
        m_autoPlaying = false;
        m_autoBtn->setText("▶ 自动播放");
    } else {
        if (m_currentStep >= m_steps.size() - 1) {
            m_currentStep = -1;
            copyState(m_state, m_steps.first().before);
            updateMatrixDisplay();
        }
        m_autoTimer->start();
        m_autoPlaying = true;
        m_autoBtn->setText("■ 停止");
    }
}
