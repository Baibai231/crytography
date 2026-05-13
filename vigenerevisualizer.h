#ifndef VIGENEREVISUALIZER_H
#define VIGENEREVISUALIZER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVector>

class VigenereVisualizer : public QDialog {
    Q_OBJECT
public:
    enum Mode { Encrypt, Decrypt };
    explicit VigenereVisualizer(Mode mode, const QString &inputText, const QString &key, QWidget *parent = nullptr);

    void startAnimation();

signals:
    void animationFinished();

private slots:
    void animateNextStep();

private:
    void setupUI();
    void updateExplanation(int step);

    Mode m_mode;
    QString m_input;   // 明文(加密模式) 或 密文(解密模式)
    QString m_key;     // 密钥
    QString m_result;  // 计算结果

    QLabel *m_explanationLabel;
    QPushButton *m_nextBtn;
    QPushButton *m_autoBtn;

    QVector<QLabel*> m_inputLabels;   // Row 0
    QVector<QLabel*> m_keyLabels;     // Row 1
    QVector<QLabel*> m_resultLabels;  // Row 2

    int m_currentStep = 0;
    QTimer *m_stepTimer = nullptr;
    bool m_autoPlay = false;

    QChar intToChar(int val) const;
    int charToInt(QChar c) const;
};

#endif
