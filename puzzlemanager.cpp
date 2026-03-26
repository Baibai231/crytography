#include "PuzzleManager.h"
#include "CaesarPuzzle.h"
#include "RailFencePuzzle.h"

#include <QDialog>

PuzzleManager::PuzzleManager(QObject *parent)
    : QObject(parent) {}

bool PuzzleManager::runPuzzle(int level, QWidget *parent) {
    if (level == 1) {
        CaesarPuzzle puzzle(parent);
        return (puzzle.exec() == QDialog::Accepted);
    }
    else if (level == 2) {
        RailFencePuzzle puzzle(parent);
        return (puzzle.exec() == QDialog::Accepted);
    }

    // 默认情况（防御性编程）
    return false;
}


