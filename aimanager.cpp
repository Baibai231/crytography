#include "aimanager.h"

#include <QJsonArray>
#include <QJsonDocument>

AIManager::AIManager(QObject *parent)
    : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);

    connect(networkManager, &QNetworkAccessManager::finished,
            this, &AIManager::onReplyFinished);
}

QString AIManager::buildSystemPrompt(const PuzzleContext &context)
{
    QString prompt = "你是一个解谜助手。\n"
                     "规则：\n"
                     "1. 只能提供提示，不能直接给出答案\n"
                     "2. 不要输出完整解密结果\n"
                     "3. 引导玩家逐步思考\n"
                     "4. 提供简短、有启发性的提示\n"
                     "5. 这是连续对话，请结合已有对话历史继续回答\n\n";

    prompt += "当前谜题类型：" + context.type + "\n";
    prompt += "谜题内容：" + context.content + "\n";
    prompt += "玩家当前进度：" + context.progress + "\n";
    prompt += "请基于后续对话历史继续给出下一步提示。";

    return prompt;
}

void AIManager::askAI(const PuzzleContext &context, const QJsonArray &conversationHistory)
{

    //debug
    if (apiKey.isEmpty()) {
        qDebug() << "ERROR: API Key is empty!";
        emit errorOccurred("API Key 未配置");
        return;
    }
    qDebug() << "apiKey in askAI:" << apiKey;


    QString systemPrompt = buildSystemPrompt(context);

    QUrl url("https://api.deepseek.com/v1/chat/completions");
    QNetworkRequest request(url);


    //debug
    qDebug() << "Request URL:" << request.url();



    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + apiKey).toUtf8());

    QJsonArray messages;

    QJsonObject systemMessage;
    systemMessage["role"] = "system";
    systemMessage["content"] = systemPrompt;
    messages.append(systemMessage);

    for (const QJsonValue &messageValue : conversationHistory) {
        if (!messageValue.isObject()) {
            continue;
        }

        const QJsonObject messageObject = messageValue.toObject();
        const QString role = messageObject["role"].toString();
        const QString content = messageObject["content"].toString();

        if (role.isEmpty() || content.isEmpty()) {
            continue;
        }

        messages.append(messageObject);
    }

    QJsonObject body;
    body["model"] = "deepseek-chat";
    body["messages"] = messages;
    body["temperature"] = 0.7;

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

    emit responseReady(content);

    reply->deleteLater();
}
void AIManager::setApiKey(const QString &key)
{
    apiKey = key;


    //debug
    qDebug() << "setApiKey called with:" << key;



}
