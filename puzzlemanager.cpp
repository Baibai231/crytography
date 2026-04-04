#include "PuzzleManager.h"
#include "CaesarPuzzle.h"
#include "RailFencePuzzle.h"

#include <QDialog>

PuzzleManager::PuzzleManager(AIManager *manager,QObject *parent)
    : QObject(parent), aiManager(manager) {}

bool PuzzleManager::runPuzzle(int level, QWidget *parent) {
    if (level == 1) {
        CaesarPuzzle puzzle(aiManager,parent);
        return (puzzle.exec() == QDialog::Accepted);
    }
    else if (level == 2) {
        RailFencePuzzle puzzle(parent);
        return (puzzle.exec() == QDialog::Accepted);
    }

    // 默认情况（防御性编程）
    return false;
}


