#include "aimanager.h"

#include <QJsonArray>
#include <QJsonDocument>
//#include <QDebug>

AIManager::AIManager(QObject *parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished,
            this, &AIManager::onReplyFinished);
}

QString AIManager::buildSystemPrompt(const GameState &state, const QString &mode)
{
    QString prompt;

    prompt += "你是一个专业的密码学解谜AI助手。\n";
    prompt += "规则：\n";
    prompt += "1. 只能提供提示，不能直接给答案\n";
    prompt += "2. 不要输出完整解密结果\n";
    prompt += "3. 引导玩家思考\n";
    prompt += "4. 回答要简短\n\n";

    prompt += "【当前游戏状态】\n";
    prompt += state.toString() + "\n";

    prompt += "【提示策略】\n";

    if (state.attemptCount == 1) {
        prompt += "当前是第1次提问：只能给模糊思路提示。\n";
    } else if (state.attemptCount == 2) {
        prompt += "当前是第2次提问：可以给出解题方向，但不要透露关键结果。\n";
    } else {
        prompt += "当前是第3次提问：可以给关键步骤，但仍然不能直接给答案。\n";
    }

    if (mode == "hint") {
        prompt += "请给出一个提示。\n";
    } else if (mode == "step") {
        prompt += "请告诉玩家下一步应该做什么。\n";
    }

    return prompt;
}

void AIManager::requestHint(const GameState &state, const QJsonArray &history)
{
    currentMode = "hint";
    QString prompt = buildSystemPrompt(state, "hint");
    sendRequest(prompt, history, "hint");
}

void AIManager::requestNextStep(const GameState &state, const QJsonArray &history)
{
    currentMode = "step";
    QString prompt = buildSystemPrompt(state, "step");
    sendRequest(prompt, history, "step");
}

void AIManager::sendRequest(const QString &prompt,
                            const QJsonArray &conversationHistory,
                            const QString &mode)
{
    if (apiKey.isEmpty()) {
        emit errorOccurred("API Key 未配置");
        return;
    }

    QUrl url("https://api.deepseek.com/v1/chat/completions");
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    QJsonArray messages;

    // system
    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = prompt;
    messages.append(systemMessage);

    // history
    for (const QJsonValue &val : conversationHistory) {
        if (val.isObject()) {
            messages.append(val.toObject());
        }
    }

    QJsonObject body;
    body["model"] = "deepseek-chat";
    body["messages"] = messages;
    body["temperature"] = 0.6;

    QJsonDocument doc(body);

    networkManager->post(request, doc.toJson());
}

void AIManager::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);

    if (!doc.isObject()) {
        emit errorOccurred("Invalid JSON response");
        reply->deleteLater();
        return;
    }

    QJsonObject obj = doc.object();
    QJsonArray choices = obj["choices"].toArray();

    if (choices.isEmpty()) {
        emit errorOccurred("No response from AI");
        reply->deleteLater();
        return;
    }

    QString content = choices[0].toObject()["message"].toObject()["content"].toString();

    // 简单防剧透处理（可扩展）
    if (content.length() > 200) {
        content = content.left(200) + "...";
    }

    if (currentMode == "hint") {
        emit hintReady(content);
    } else if (currentMode == "step") {
        emit nextStepReady(content);
    }

    reply->deleteLater();
}
void AIManager::setApiKey(const QString &key)
{
    apiKey = key;


    //debug
    qDebug() << "setApiKey called with:" << key;



}
