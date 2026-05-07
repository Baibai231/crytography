#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <QString>


// 游戏状态：用于传递给 AI 的上下文信息
class GameState
{
public:
    GameState();

    // ===== 基本信息 =====
    QString puzzleType;      // 谜题类型："caesar" / "rail_fence"
    QString encryptedText;   // 当前密文
    QString userInput;       // 用户当前输入

    // ===== 状态信息 =====
    int attemptCount;        // 尝试次数
    bool solved;             // 是否已解

    int difficulty;
    QString hintLevel;

    // ===== 工具函数 =====
    QString toString() const;   // 转为字符串（用于调试 / prompt）
};

#endif // GAMESTATE_H
