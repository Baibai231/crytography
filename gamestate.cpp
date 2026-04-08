#include "gamestate.h"

GameState::GameState()
{
    puzzleType = "";
    encryptedText = "";
    userInput = "";

    attemptCount = 0;
    solved = false;

    difficulty = 1;
    hintLevel = "low";
}

// 🔥 用于调试 & 构造 prompt
QString GameState::toString() const
{
    QString info;

    info += "谜题类型: " + puzzleType + "\n";
    info += "密文: " + encryptedText + "\n";
    info += "用户输入: " + userInput + "\n";
    info += "尝试次数: " + QString::number(attemptCount) + "\n";
    info += "是否已解: " + QString(solved ? "是" : "否") + "\n";
    info += "难度: " + QString::number(difficulty) + "\n";
    info += "提示等级: " + hintLevel + "\n";

    return info;
}
