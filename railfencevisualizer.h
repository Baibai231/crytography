#pragma once

#include <QDialog>
#include <QLabel>
#include <QVector>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QTimer>
#include <QDial>


class CipherTile : public QLabel {
public:
    CipherTile(const QString &text, QWidget *parent = nullptr);
};

class FenceSlot : public QLabel {
public:
    explicit FenceSlot(QWidget *parent = nullptr);
    void setChar(const QString &ch, bool highlight = false);
    void reset();
};

class RailFenceVisualizer : public QDialog {
    Q_OBJECT

public:
    explicit RailFenceVisualizer(const QString &cipher, QWidget *parent = nullptr);

    void showResult();

private slots:
    void play();
    void pause();
    void reset();
    void playNextStep();
    void onSliderChanged(int value);
    void onKeyChanged(int value);

private:
    struct Step {
        int row;
        int col;
        QString ch;
    };

private:
    void buildUI();
    void buildFence();
    void buildSteps();
    void updateResultDisplay();
    QString buildDecodedResult() const;

private:
    QString cipherText;
    QString targetText;
    int key;

    QVector<QVector<FenceSlot*>> fenceSlots;
    QVector<CipherTile*> tiles;

    QVector<Step> steps;
    int currentStep = 0;

    QVBoxLayout *fenceLayout;
    QSlider *slider;
    QLabel *resultLabel;

    QTimer *animTimer;

    QDial *keyDial;

    bool isPlaying = false;

    QVector<QPair<int,int>> path;
};
