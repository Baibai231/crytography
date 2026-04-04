#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

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

    void askAI(const PuzzleContext &context, const QString &playerInput);

signals:
    void responseReady(const QString &response);
    void errorOccurred(const QString &error);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QString buildPrompt(const PuzzleContext &context, const QString &playerInput);

private:
    QNetworkAccessManager *networkManager;
    QString apiKey;
};

#endif // AIMANAGER_H
