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
    void checkPart1();
    void checkPart2();
    void showEncryptVisualizer();
    void showDecryptVisualizer();
    void requestHint();

private:
    void generatePuzzle();
    void setupPart1();
    void setupPart2();
    void advanceToPart2();
    void shakeWindow();

    int m_plainText[2][2];
    int m_key[2][2];
    int m_cipherText[2][2];

    int m_decryptCipher[2][2];
    int m_decryptKey[2][2];

    QLineEdit *m_inputEdit1;
    QLineEdit *m_inputEdit2;

    QStackedWidget *m_stack;

    AIManager *aiManager;
    GameState gameState;

    QString m_part1Expected;
    QString m_part2Expected;

    QWidget* createMatrixWidget(const QString &label, int data[2][2], const QString &accentColor);
};

#endif
