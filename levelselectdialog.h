#ifndef LEVELSELECTDIALOG_H
#define LEVELSELECTDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QPoint>
#include <QVector>

class LevelSelectDialog : public QDialog {
    Q_OBJECT

public:
    explicit LevelSelectDialog(QWidget *parent = nullptr);

signals:
    void levelSelected(int level);

private:
    void updateButtonStyles();
    void resetProgress();
    void handleLevelClick(int level);
    void paintEvent(QPaintEvent *event) override;

    QPushButton *m_lvl1Btn;
    QPushButton *m_lvl2Btn;
    QPushButton *m_lvl3Btn;
    QPushButton *m_lvl4Btn;
    QPushButton *m_resetBtn;
    QWidget *m_mapArea;

    QVector<QPoint> m_buttonCenters;
};

#endif
