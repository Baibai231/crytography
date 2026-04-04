#ifndef PUZZLEMANAGER_H
#define PUZZLEMANAGER_H

#include <QObject>
#include "aimanager.h"

class QWidget;

class PuzzleManager : public QObject {
    Q_OBJECT

private:
     AIManager *aiManager;

public:
    explicit PuzzleManager(AIManager *manager,QObject *parent = nullptr);

    // 运行当前关卡对应的谜题
    bool runPuzzle(int level, QWidget *parent);
};

#endif
