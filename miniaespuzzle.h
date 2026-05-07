#ifndef MINIAESPUZZLE_H
#define MINIAESPUZZLE_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

#include "aimanager.h"
#include "gamestate.h"
#include "miniaesvisualizer.h"

class MiniAESPuzzle : public QDialog {
    Q_OBJECT

public:
    explicit MiniAESPuzzle(AIManager *aiManager, QWidget *parent = nullptr);

private slots:
    void checkPhase1();
    void checkPhase2();
    void checkPhase3();
    void showVisualizer();
    void requestHint();

private:
    void generatePuzzle();
    void setupPhase1();
    void setupPhase2();
    void setupPhase3();
    void advanceToPhase(int phase);
    void shakeWindow();

    int m_plainText[2][2];
    int m_key[2][2];
    int m_cipherText[2][2];

    int m_phase2Plain[2][2];
    int m_phase2Key[2][2];
    int m_phase3Cipher[2][2];
    int m_phase3Key[2][2];

    QList<AesStep> m_steps;

    QLineEdit *m_inputEdit1;
    QLineEdit *m_inputEdit2;
    QLineEdit *m_inputEdit3;

    QStackedWidget *m_stack;

    AIManager *aiManager;
    GameState gameState;

    QString m_phase1Expected;
    QString m_phase2Expected;
    QString m_phase3Expected;

    QWidget* createMatrixWidget(const QString &label, int data[2][2], const QString &accentColor);
};

#endif
