#ifndef PUZZLEMANAGER_H
#define PUZZLEMANAGER_H

#include <QObject>

class QWidget;

class PuzzleManager : public QObject {
    Q_OBJECT

public:
    explicit PuzzleManager(QObject *parent = nullptr);

    // 运行当前关卡对应的谜题
    bool runPuzzle(int level, QWidget *parent);
};

#endif
