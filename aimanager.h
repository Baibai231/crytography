#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include "gamestate.h"

struct PuzzleContext {
    QString type;      // e.g. "caesar"
    QString content;   // encrypted text
    QString progress;  // optional player progress
};

class AIManager : public QObject
{
    Q_OBJECT
public:
    explicit AIManager(QObject *parent = nullptr);

    void setApiKey(const QString &key);

    void requestHint(const GameState &state, const QJsonArray &history);
    void requestNextStep(const GameState &state, const QJsonArray &history);


signals:
    void hintReady(const QString &hint);
    void nextStepReady(const QString &step);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString buildSystemPrompt(const GameState &state, const QString &mode);
    void sendRequest(const QString &prompt,
                     const QJsonArray &history,
                     const QString &mode);

private:
    QNetworkAccessManager *networkManager;
    QString apiKey;

    QString currentMode;//标记当前请求类型（hint / step）
};

#endif // AIMANAGER_H
