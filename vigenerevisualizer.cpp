#include "VigenereVisualizer.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>
#include <QStyle>

VigenereVisualizer::VigenereVisualizer(Mode mode, const QString &inputText, const QString &key, QWidget *parent)
    : QDialog(parent), m_mode(mode), m_input(inputText), m_key(key), m_currentStep(0), m_autoPlay(false) {

    setWindowTitle(mode == Encrypt ? "维吉尼亚加密推演" : "维吉尼亚解密推演");
    setFixedSize(700, 550);
    setStyleSheet(R"(
        QDialog { background-color: #151920; }
        QLabel { color: #f0e8d8; font-family: 'Microsoft YaHei'; }
        QLabel[role="title"] { font-size: 20px; font-weight: 700; color: #e4b45f; }
        QLabel[role="input"] {
            background-color: #2c3540; border: 2px solid #4a6075; border-radius: 8px;
            color: #8ac4ff; font-size: 28px; font-weight: 700; padding: 8px;
        }
        QLabel[role="key"] {
            background-color: #2c3540; border: 2px solid #4a6075; border-radius: 8px;
            color: #66e078; font-size: 28px; font-weight: 700; padding: 8px;
        }
        QLabel[role="result"] {
            background-color: #2c3540; border: 2px solid #6b5638; border-radius: 8px;
            color: #ffd58a; font-size: 28px; font-weight: 700; padding: 8px;
        }
        QLabel[role="input-active"] { background-color: #3a4a5a; border-color: #8ac4ff; }
        QLabel[role="key-active"] { background-color: #3a4a5a; border-color: #66e078; }
        QLabel[role="result-active"] { background-color: #3a352a; border-color: #ffd58a; }
        QLabel[role="explain"] {
            font-size: 20px; color: #e2d5bd; background-color: rgba(44, 31, 22, 180);
            border: 1px solid rgba(228, 180, 95, 80); border-radius: 10px; padding: 15px;
        }
        QPushButton {
            background-color: #d7a84e; color: #1f160e; font-weight: 700;
            min-height: 40px; border-radius: 8px; font-size: 15px; border: none;
        }
        QPushButton:hover { background-color: #ecc166; }
        QPushButton:pressed { background-color: #c8973f; }
        QPushButton[variant="secondary"] {
            background-color: rgba(78, 58, 40, 215); color: #f7eedb;
            border: 1px solid rgba(234, 197, 141, 90);
        }
    )");

    setupUI();
}

void VigenereVisualizer::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);

    // Title
    QLabel *title = new QLabel(m_mode == Encrypt ? "加密演示：以钥铸锁" : "解密演示：以钥启明");
    title->setProperty("role", "title");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Grid for 3 rows
    QGridLayout *grid = new QGridLayout();
    grid->setSpacing(10);
    grid->setContentsMargins(0, 0, 0, 0);

    int len = m_input.length();

    // Row 0: Input
    for (int i = 0; i < len; ++i) {
        QLabel *lbl = new QLabel(QString(m_input[i]));
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setProperty("role", "input");
        lbl->setFixedSize(60, 60);
        m_inputLabels.append(lbl);
        grid->addWidget(lbl, 0, i, Qt::AlignCenter);
    }

    // Row 1: Key (auto-repeat)
    for (int i = 0; i < len; ++i) {
        QChar k = m_key[i % m_key.length()];
        QLabel *lbl = new QLabel(QString(k));
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setProperty("role", "key");
        lbl->setFixedSize(60, 60);
        m_keyLabels.append(lbl);
        grid->addWidget(lbl, 1, i, Qt::AlignCenter);
    }

    // Row 2: Result (starts with ?)
    m_result = QString(len, '?');
    for (int i = 0; i < len; ++i) {
        QLabel *lbl = new QLabel("?");
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setProperty("role", "result");
        lbl->setFixedSize(60, 60);
        m_resultLabels.append(lbl);
        grid->addWidget(lbl, 2, i, Qt::AlignCenter);
    }

    mainLayout->addLayout(grid);
    mainLayout->addSpacing(10);

    // Explanation Box
    m_explanationLabel = new QLabel("点击「下一步」或「自动推演」开始...", this);
    m_explanationLabel->setProperty("role", "explain");
    m_explanationLabel->setWordWrap(true);
    m_explanationLabel->setAlignment(Qt::AlignCenter);
    m_explanationLabel->setMinimumHeight(80);
    mainLayout->addWidget(m_explanationLabel);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(12);

    m_nextBtn = new QPushButton("下一步");
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    connect(m_nextBtn, &QPushButton::clicked, this, &VigenereVisualizer::animateNextStep);

    m_autoBtn = new QPushButton("自动推演");
    m_autoBtn->setProperty("variant", "secondary");
    m_autoBtn->setCursor(Qt::PointingHandCursor);
    connect(m_autoBtn, &QPushButton::clicked, this, [=]() {
        m_autoPlay = true;
        if (m_stepTimer) m_stepTimer->stop();
        m_stepTimer = new QTimer(this);
        connect(m_stepTimer, &QTimer::timeout, this, &VigenereVisualizer::animateNextStep);
        m_stepTimer->start(1500);
        m_nextBtn->setEnabled(false);
        m_autoBtn->setEnabled(false);
    });

    btnLayout->addWidget(m_nextBtn);
    btnLayout->addWidget(m_autoBtn);
    mainLayout->addLayout(btnLayout);
}

void VigenereVisualizer::animateNextStep() {
    if (m_currentStep >= m_input.length()) {
        if (m_stepTimer) m_stepTimer->stop();
        emit animationFinished();
        return;
    }

    int i = m_currentStep;
    int p = charToInt(m_input[i]);
    int k = charToInt(m_key[i % m_key.length()]);

    // Highlight current column
    for (int c = 0; c < m_input.length(); ++c) {
        if (c == i) {
            m_inputLabels[c]->setProperty("role", "input-active");
            m_keyLabels[c]->setProperty("role", "key-active");
            m_resultLabels[c]->setProperty("role", "result-active");
        } else {
            m_inputLabels[c]->setProperty("role", "input");
            m_keyLabels[c]->setProperty("role", "key");
            if (c < m_currentStep) {
                m_resultLabels[c]->setProperty("role", "result");
            }
        }
        m_inputLabels[c]->style()->unpolish(m_inputLabels[c]);
        m_inputLabels[c]->style()->polish(m_inputLabels[c]);
        m_keyLabels[c]->style()->unpolish(m_keyLabels[c]);
        m_keyLabels[c]->style()->polish(m_keyLabels[c]);
        m_resultLabels[c]->style()->unpolish(m_resultLabels[c]);
        m_resultLabels[c]->style()->polish(m_resultLabels[c]);
    }

    // Calculate result
    int res = 0;
    char operation = '+';
    if (m_mode == Encrypt) {
        res = (p + k) % 26;
        operation = '+';
    } else {
        res = (p - k + 26) % 26;
        operation = '-';
    }

    // Update explanation
    updateExplanation(i);

    // Fill result
    QChar resChar = intToChar(res);
    m_resultLabels[i]->setText(QString(resChar));

    m_currentStep++;

    if (m_currentStep >= m_input.length()) {
        if (m_stepTimer) m_stepTimer->stop();
        QTimer::singleShot(800, this, [this]() {
            m_explanationLabel->setText("✨ 推演完成！所有字母已计算完毕。");
            emit animationFinished();
        });
    }
}

void VigenereVisualizer::updateExplanation(int step) {
    int p = charToInt(m_input[step]);
    int k = charToInt(m_key[step % m_key.length()]);
    int res;
    QString formula;

    if (m_mode == Encrypt) {
        res = (p + k) % 26;
        formula = QString("<b>步骤 %1</b><br>%2 (%3) <span style='color:#66e078;'>+</span> %4 (%5) = %6 mod 26 = <span style='color:#ffd58a;'>%7 → %8</span>")
                      .arg(step + 1)
                      .arg(m_input[step]).arg(p)
                      .arg(m_key[step % m_key.length()]).arg(k)
                      .arg(p + k)
                      .arg(res)
                      .arg(intToChar(res));
    } else {
        res = (p - k + 26) % 26;
        formula = QString("<b>步骤 %1</b><br>%2 (%3) <span style='color:#66e078;'>-</span> %4 (%5) = %6 mod 26 = <span style='color:#ffd58a;'>%7 → %8</span>")
                      .arg(step + 1)
                      .arg(m_input[step]).arg(p)
                      .arg(m_key[step % m_key.length()]).arg(k)
                      .arg(p - k)
                      .arg(res)
                      .arg(intToChar(res));
    }

    m_explanationLabel->setText(formula);
}

void VigenereVisualizer::startAnimation() {
    m_currentStep = 0;
    // Reset results
    for (auto lbl : m_resultLabels) {
        lbl->setText("?");
        lbl->setProperty("role", "result");
        lbl->style()->unpolish(lbl);
        lbl->style()->polish(lbl);
    }
    animateNextStep();
}

QChar VigenereVisualizer::intToChar(int val) const {
    return QChar('A' + val);
}

int VigenereVisualizer::charToInt(QChar c) const {
    return c.toUpper().toLatin1() - 'A';
}
