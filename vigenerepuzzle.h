#ifndef VIGENEREPUZZLE_H
#define VIGENEREPUZZLE_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

#include "aimanager.h"
#include "gamestate.h"
#include "VigenereVisualizer.h"

class VigenerePuzzle : public QDialog {
    Q_OBJECT
public:
    explicit VigenerePuzzle(AIManager *aiManager, QWidget *parent = nullptr);

private slots:
    void checkPhase1();
    void checkPhase2();
    void requestHint();
    void showVisualizer(VigenereVisualizer::Mode mode, const QString &input, const QString &key);

private:
    void setupPhase1();
    void setupPhase2();

    QString m_phase1Answer;
    QString m_phase1Plain;
    QString m_phase1Key;

    QString m_phase2Answer;
    QString m_phase2Cipher;
    QString m_phase2Key;

    QLineEdit *m_inputEdit1;
    QLineEdit *m_inputEdit2;
    QStackedWidget *m_stack;

    AIManager *aiManager;
    GameState gameState;

    void shakeWindow();
};

#endif
