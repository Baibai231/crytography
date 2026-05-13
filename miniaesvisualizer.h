#ifndef MINIAESVISUALIZER_H
#define MINIAESVISUALIZER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QTimer>

struct AesStep {
    QString name;
    QString detail;
    int before[2][2];
    int after[2][2];
};

class MiniAESVisualizer : public QDialog {
    Q_OBJECT

public:
    enum Mode { Demo, Interactive, Encrypt, Decrypt };
    explicit MiniAESVisualizer(int plain[2][2], int key[2][2], Mode mode = Demo, QWidget *parent = nullptr);

    const QList<AesStep>& getSteps() const { return m_steps; }

    static const int SBOX[256];
    static const int INVSBOX[256];
    static QString toHex(int v);
    static void encrypt(int plain[2][2], int key[2][2], int result[2][2]);
    static void decrypt(int cipher[2][2], int key[2][2], int result[2][2]);
    static QList<AesStep> generateSteps(int plain[2][2], int key[2][2]);
    static QList<AesStep> generateDecryptSteps(int cipher[2][2], int key[2][2]);

private slots:
    void prevStep();
    void nextStep();
    void toggleAutoPlay();
    void showSBoxPopup();

private:
    void setupUI();
    void updateMatrixDisplay();
    void updateStepDisplay();
    void highlightCells(const int cells[4][2], int count);

    int m_state[2][2];
    int m_key[2][2];
    int m_currentStep = -1;
    Mode m_mode;

    QList<AesStep> m_steps;

    QLabel *m_matrixLabels[2][2];
    QLabel *m_stepNameLabel;
    QLabel *m_stepDetailLabel;
    QLabel *m_stepCounterLabel;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;
    QPushButton *m_autoBtn;

    QTimer *m_autoTimer;
    bool m_autoPlaying = false;
};

#endif
