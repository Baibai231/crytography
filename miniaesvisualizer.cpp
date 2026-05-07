#include "miniaesvisualizer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QPropertyAnimation>
#include <QTimer>
#include <QStyle>

const int MiniAESVisualizer::SBOX[16] = {
    0x9, 0x4, 0xA, 0xB,
    0xD, 0x1, 0x8, 0x5,
    0x6, 0x2, 0x0, 0x3,
    0xC, 0xE, 0xF, 0x7
};

static const int RCON[3][2][2] = {
    {{0x0, 0x0}, {0x0, 0x0}},
    {{0x9, 0x0}, {0x0, 0x0}},
    {{0xA, 0x0}, {0x0, 0x0}}
};

QString MiniAESVisualizer::toHex(int v) {
    const char* hex = "0123456789ABCDEF";
    return QString(hex[v & 0xF]);
}

static void copyState(int dst[2][2], const int src[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            dst[i][j] = src[i][j];
}

static void subNib(int state[2][2]) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            state[i][j] = MiniAESVisualizer::SBOX[state[i][j] & 0xF];
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
    subNib(state);
    shiftRows(state);
    mixColumns(state);
    addRoundKey(state, rk);

    getRoundKey(key, rk, 2);
    subNib(state);
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

    // Round 0: AddRoundKey
    getRoundKey(key, rk, 0);
    copyState(before, state);
    step.name = "Round 0: AddRoundKey";
    step.detail = QString("State XOR RoundKey0\n")
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

    // Round 1: SubNib
    getRoundKey(key, rk, 1);
    copyState(before, state);
    step.name = "Round 1: SubNib";
    step.detail = QString("SBox substitution\n")
        + QString("SBox[%1]=%2  SBox[%3]=%4\n")
              .arg(toHex(before[0][0])).arg(toHex(SBOX[before[0][0]]))
              .arg(toHex(before[0][1])).arg(toHex(SBOX[before[0][1]]))
        + QString("SBox[%1]=%2  SBox[%3]=%4")
              .arg(toHex(before[1][0])).arg(toHex(SBOX[before[1][0]]))
              .arg(toHex(before[1][1])).arg(toHex(SBOX[before[1][1]]));
    copyState(step.before, before);
    subNib(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: ShiftRows
    copyState(before, state);
    step.name = "Round 1: ShiftRows";
    step.detail = QString("Row 1 left shift\n")
        + QString("[%1 %2] → [%3 %4]")
              .arg(toHex(before[1][0])).arg(toHex(before[1][1]))
              .arg(toHex(before[1][1])).arg(toHex(before[1][0]));
    copyState(step.before, before);
    shiftRows(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: MixColumns
    copyState(before, state);
    step.name = "Round 1: MixColumns";
    step.detail = QString("Column mixing (XOR)\n")
        + QString("%1⊕%2=%3  %4⊕%5=%6\n")
              .arg(toHex(before[0][0])).arg(toHex(before[1][0])).arg(toHex(before[0][0]^before[1][0]))
              .arg(toHex(before[0][1])).arg(toHex(before[1][1])).arg(toHex(before[0][1]^before[1][1]))
        + QString("Old top becomes bottom");
    copyState(step.before, before);
    mixColumns(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 1: AddRoundKey
    copyState(before, state);
    step.name = "Round 1: AddRoundKey";
    step.detail = QString("State XOR RoundKey1\n")
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

    // Round 2: SubNib
    getRoundKey(key, rk, 2);
    copyState(before, state);
    step.name = "Round 2: SubNib";
    step.detail = QString("SBox substitution\n")
        + QString("SBox[%1]=%2  SBox[%3]=%4\n")
              .arg(toHex(before[0][0])).arg(toHex(SBOX[before[0][0]]))
              .arg(toHex(before[0][1])).arg(toHex(SBOX[before[0][1]]))
        + QString("SBox[%1]=%2  SBox[%3]=%4")
              .arg(toHex(before[1][0])).arg(toHex(SBOX[before[1][0]]))
              .arg(toHex(before[1][1])).arg(toHex(SBOX[before[1][1]]));
    copyState(step.before, before);
    subNib(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 2: ShiftRows
    copyState(before, state);
    step.name = "Round 2: ShiftRows";
    step.detail = QString("Row 1 left shift\n")
        + QString("[%1 %2] → [%3 %4]")
              .arg(toHex(before[1][0])).arg(toHex(before[1][1]))
              .arg(toHex(before[1][1])).arg(toHex(before[1][0]));
    copyState(step.before, before);
    shiftRows(state);
    copyState(step.after, state);
    steps.append(step);

    // Round 2: AddRoundKey
    copyState(before, state);
    step.name = "Round 2: AddRoundKey (Final)";
    step.detail = QString("State XOR RoundKey2 — 最终密文！\n")
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

    setWindowTitle("Mini-AES 矩阵推演");
    setFixedSize(620, 560);
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
        QLabel[role="sbox"] {
            background-color: #1a1e26; border: 1px solid #2a2520; border-radius: 4px;
            color: #a09880; font-size: 12px; text-align: center;
            font-family: Consolas, 'Courier New', monospace;
        }
        QLabel[role="sbox-header"] { color: #8a7e6a; font-size: 11px; }
        QPushButton {
            background-color: #d7a84e; color: #1f160e; font-weight: 700;
            min-height: 36px; border-radius: 8px; font-size: 14px; border: none;
        }
        QPushButton:hover { background-color: #ecc166; }
        QPushButton:pressed { background-color: #c8973f; }
        QPushButton:disabled { background-color: #3a3530; color: #666; }
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

    // Main content: matrix left, sbox right
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(16);

    // Left: State Matrix
    QWidget *matrixWidget = new QWidget(this);
    QVBoxLayout *matrixLayout = new QVBoxLayout(matrixWidget);
    matrixLayout->setContentsMargins(0, 0, 0, 0);
    matrixLayout->setSpacing(8);

    QLabel *matrixTitle = new QLabel("State Matrix", matrixWidget);
    matrixTitle->setProperty("role", "subtitle");
    matrixTitle->setAlignment(Qt::AlignCenter);
    matrixLayout->addWidget(matrixTitle);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            m_matrixLabels[i][j] = new QLabel("0", matrixWidget);
            m_matrixLabels[i][j]->setProperty("role", "matrix");
            m_matrixLabels[i][j]->setFixedSize(60, 60);
            m_matrixLabels[i][j]->setAlignment(Qt::AlignCenter);
            gridLayout->addWidget(m_matrixLabels[i][j], i, j);
        }
    }
    matrixLayout->addLayout(gridLayout);
    contentLayout->addWidget(matrixWidget);

    // Right: SBox reference
    QFrame *sboxPanel = new QFrame(this);
    sboxPanel->setProperty("role", "panel");
    QVBoxLayout *sboxLayout = new QVBoxLayout(sboxPanel);
    sboxLayout->setContentsMargins(12, 10, 12, 10);
    sboxLayout->setSpacing(6);

    QLabel *sboxTitle = new QLabel("S-Box 查找表", sboxPanel);
    sboxTitle->setProperty("role", "subtitle");
    sboxTitle->setAlignment(Qt::AlignCenter);
    sboxLayout->addWidget(sboxTitle);

    QGridLayout *sboxGrid = new QGridLayout();
    sboxGrid->setSpacing(2);
    // Header row
    QLabel *cornerLabel = new QLabel("x\\y", sboxPanel);
    cornerLabel->setProperty("role", "sbox-header");
    cornerLabel->setAlignment(Qt::AlignCenter);
    sboxGrid->addWidget(cornerLabel, 0, 0);
    for (int j = 0; j < 4; j++) {
        QLabel *h = new QLabel(QString::number(j), sboxPanel);
        h->setProperty("role", "sbox-header");
        h->setAlignment(Qt::AlignCenter);
        sboxGrid->addWidget(h, 0, j + 1);
    }
    // Header column
    for (int i = 0; i < 4; i++) {
        QLabel *v = new QLabel(QString("%1x").arg(i), sboxPanel);
        v->setProperty("role", "sbox-header");
        v->setAlignment(Qt::AlignCenter);
        sboxGrid->addWidget(v, i + 1, 0);
    }
    // SBox values
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int val = SBOX[i * 4 + j];
            m_sboxLabels[i][j] = new QLabel(toHex(val), sboxPanel);
            m_sboxLabels[i][j]->setProperty("role", "sbox");
            m_sboxLabels[i][j]->setFixedSize(28, 28);
            m_sboxLabels[i][j]->setAlignment(Qt::AlignCenter);
            sboxGrid->addWidget(m_sboxLabels[i][j], i + 1, j + 1);
        }
    }
    sboxLayout->addLayout(sboxGrid);
    contentLayout->addWidget(sboxPanel);

    mainLayout->addLayout(contentLayout);

    // Step display panel
    QFrame *stepPanel = new QFrame(this);
    stepPanel->setProperty("role", "panel");
    QVBoxLayout *stepLayout = new QVBoxLayout(stepPanel);
    stepLayout->setContentsMargins(16, 14, 16, 14);
    stepLayout->setSpacing(8);

    QHBoxLayout *stepHeaderLayout = new QHBoxLayout();
    m_stepNameLabel = new QLabel("—", stepPanel);
    m_stepNameLabel->setProperty("role", "step-name");
    m_stepCounterLabel = new QLabel("— / 8", stepPanel);
    m_stepCounterLabel->setProperty("role", "counter");
    m_stepCounterLabel->setAlignment(Qt::AlignRight);
    stepHeaderLayout->addWidget(m_stepNameLabel);
    stepHeaderLayout->addWidget(m_stepCounterLabel);
    stepLayout->addLayout(stepHeaderLayout);

    m_stepDetailLabel = new QLabel("点击下一步开始推演", stepPanel);
    m_stepDetailLabel->setProperty("role", "step-detail");
    m_stepDetailLabel->setWordWrap(true);
    stepLayout->addWidget(m_stepDetailLabel);

    mainLayout->addWidget(stepPanel);

    // Controls
    QHBoxLayout *ctrlLayout = new QHBoxLayout();
    ctrlLayout->setSpacing(10);

    m_prevBtn = new QPushButton("← 上一步", this);
    m_prevBtn->setProperty("variant", "secondary");
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
