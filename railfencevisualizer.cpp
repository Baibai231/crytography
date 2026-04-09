#include "RailFenceVisualizer.h"

// ================= CipherTile =================
CipherTile::CipherTile(const QString &text, QWidget *parent)
    : QLabel(text, parent) {
    setFixedSize(40, 40);
    setAlignment(Qt::AlignCenter);
    setStyleSheet("background:#444;color:white;border-radius:6px;");
}

// ================= FenceSlot =================
FenceSlot::FenceSlot(QWidget *parent)
    : QLabel(parent) {
    setFixedSize(40, 40);
    setAlignment(Qt::AlignCenter);
    setStyleSheet("border:1px dashed gray;");
}

void FenceSlot::setChar(const QString &ch, bool highlight) {
    setText(ch);

    if (ch.isEmpty()) {
        setStyleSheet("border:1px dashed gray;");
        return;
    }

    if (highlight) {
        setStyleSheet("background:#ffcc66;color:black;border-radius:6px;");
    } else {
        setStyleSheet("background:#666;color:white;border-radius:6px;");
    }
}

void FenceSlot::reset() {
    setText("");
    setStyleSheet("border:1px dashed gray;");
}

// ================= Constructor =================
RailFenceVisualizer::RailFenceVisualizer(const QString &cipher, QWidget *parent)
    : QDialog(parent),
    cipherText(cipher),
    key(2),
    targetText("HELLOWORLD")
{
    setWindowTitle("栅栏密码可视化");
    setFixedSize(750, 500);

    animTimer = new QTimer(this);
    sliderTimer = new QTimer(this);

    sliderTimer->setSingleShot(true);

    connect(animTimer, &QTimer::timeout, this, &RailFenceVisualizer::playNextStep);

    buildUI();
    buildFence();
    buildSteps();
}

// ================= UI =================
void RailFenceVisualizer::buildUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // ================= 顶部密文 =================
    QHBoxLayout *topLayout = new QHBoxLayout();

    for (QChar c : cipherText) {
        CipherTile *tile = new CipherTile(QString(c), this);
        tiles.push_back(tile);
        topLayout->addWidget(tile);
    }

    mainLayout->addLayout(topLayout);

    // ================= 栅栏数选择（🔥新增） =================
    QHBoxLayout *controlLayout = new QHBoxLayout();

    QLabel *label = new QLabel("栅栏数：");

    keyDial = new QDial(this);
    keyDial->setRange(2, 5);
    keyDial->setValue(2);
    keyDial->setNotchesVisible(true);   // ⭐刻度点
    keyDial->setFixedSize(80, 80);

    QLabel *valueLabel = new QLabel("2");

    connect(keyDial, &QDial::valueChanged, this, [=](int v) {
        valueLabel->setText(QString::number(v));
        onKeyChanged(v);
    });

    controlLayout->addWidget(label);
    controlLayout->addWidget(keyDial);
    controlLayout->addWidget(valueLabel);

    mainLayout->addLayout(controlLayout);

    // ================= 栅栏区域 =================
    fenceLayout = new QVBoxLayout();
    mainLayout->addLayout(fenceLayout);

    // ================= slider =================
    slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, cipherText.size());

    connect(slider, &QSlider::valueChanged, this, [=](int v) {
        sliderTimer->start(30);

        connect(sliderTimer, &QTimer::timeout, this, [=]() {
            onSliderChanged(v);
        });
    });

    mainLayout->addWidget(slider);

    // ================= 结果显示（🔥新增） =================
    resultLabel = new QLabel("结果：");
    resultLabel->setStyleSheet("font-size:16px;color:#ddd;");
    mainLayout->addWidget(resultLabel);
}

// ================= buildFence =================
void RailFenceVisualizer::buildFence()
{
    for (auto &row : fenceSlots)
        for (auto slot : row)
            slot->deleteLater();

    fenceSlots.clear();

    fenceSlots.resize(key);

    int cols = cipherText.size();

    for (int r = 0; r < key; r++) {
        QHBoxLayout *row = new QHBoxLayout();

        for (int c = 0; c < cols; c++) {
            FenceSlot *slot = new FenceSlot(this);
            fenceSlots[r].push_back(slot);
            row->addWidget(slot);
        }

        fenceLayout->addLayout(row);
    }
}

// ================= buildSteps（核心） =================
void RailFenceVisualizer::buildSteps()
{
    steps.clear();
    currentStep = 0;

    int n = cipherText.size();

    QVector<QVector<bool>> mark(key, QVector<bool>(n, false));

    int row = 0, dir = 1;

    for (int col = 0; col < n; col++) {
        mark[row][col] = true;

        if (row == 0) dir = 1;
        else if (row == key - 1) dir = -1;

        row += dir;
    }

    int index = 0;

    for (int r = 0; r < key; r++) {
        for (int c = 0; c < n; c++) {
            if (mark[r][c] && index < n) {
                steps.push_back({r, c, QString(cipherText[index])});
                index++;
            }
        }
    }

    slider->setRange(0, steps.size());

    path.clear();

    for (int col = 0; col < n; col++) {

        path.push_back({row, col});

        if (row == 0) dir = 1;
        else if (row == key - 1) dir = -1;

        row += dir;
    }
}

// ================= 动画播放 =================
void RailFenceVisualizer::play()
{
    if (isPlaying) return;

    isPlaying = true;
    animTimer->start(200);
}

void RailFenceVisualizer::pause()
{
    isPlaying = false;
    animTimer->stop();
}

void RailFenceVisualizer::reset()
{
    pause();

    currentStep = 0;

    for (auto &row : fenceSlots)
        for (auto slot : row)
            slot->reset();

    slider->setValue(0);
}

// ================= 下一步 =================
void RailFenceVisualizer::playNextStep()
{
    if (currentStep >= steps.size()) {

        animTimer->stop();
        isPlaying = false;

        QString result;

        int n = cipherText.size();

        int row = 0, dir = 1;

        for (int col = 0; col < n; col++) {

            QString ch = fenceSlots[row][col]->text();

            // 如果没有字符 → 空白
            if (ch.isEmpty()) {
                ch = " ";
            }

            result += ch;

            // =================🔥 亮暗判断 =================
            if (!ch.isEmpty() && col < steps.size()) {
                fenceSlots[row][col]->setStyleSheet(
                    "background:#ffcc66;color:black;border-radius:6px;"
                    );
            } else {
                fenceSlots[row][col]->setStyleSheet(
                    "background:#555;color:#999;border-radius:6px;"
                    );
            }

            // W路径移动
            if (row == 0) dir = 1;
            else if (row == key - 1) dir = -1;

            row += dir;
        }

        resultLabel->setText("结果：" + result);
        return;
    }

    const Step &s = steps[currentStep];

    fenceSlots[s.row][s.col]->setChar(s.ch, true);

    currentStep++;

    slider->setValue(currentStep);


    if (currentStep >= steps.size()) {
        animTimer->stop();
        isPlaying = false;

        showResult();   // ⭐关键调用
        return;
    }
}

void RailFenceVisualizer::showResult()
{
    QString result;

    for (auto &p : path) {

        int r = p.first;
        int c = p.second;

        QString ch = fenceSlots[r][c]->text();

        result += ch.isEmpty() ? " " : ch;
    }

    resultLabel->setText("结果：" + result);
}

// ================= slider控制 =================
void RailFenceVisualizer::onSliderChanged(int value)
{
    currentStep = value;

    for (auto &row : fenceSlots)
        for (auto slot : row)
            slot->reset();

    for (int i = 0; i < currentStep && i < steps.size(); i++) {
        const Step &s = steps[i];
        fenceSlots[s.row][s.col]->setChar(s.ch, true);
    }
}

// ================= key变化 =================
void RailFenceVisualizer::onKeyChanged(int value)
{
    key = value;

    reset();

    buildFence();
    buildSteps();

    // 🔥 核心：刷新结果显示
    resultLabel->setText("结果：");
}

