#pragma once

#include <iostream>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QUrl>
#include <QString>

class GoogleSSO : public QOAuth2AuthorizationCodeFlow {

    Q_OBJECT

public:
    GoogleSSO(const QString&, QObject* = nullptr);
    virtual ~GoogleSSO();

    QString clientId() const;
    void setClientId(const QString&);

    QString clientSecret() const;
    void setClientSecret(const QString&);

    void saveTokens();
    void loadTokens();

public slots:
    void authenticate();
    void onRequestFailed(QAbstractOAuth::Error);
    void onStatusChanged(QAbstractOAuth::Status);

protected:

    void timerEvent(QTimerEvent*) override;

private:

    // URIs
    static const QUrl AUTHORIZATION_CODE_URI;
    static const QUrl ACCESS_TOKEN_URI;

    // Main objects used for networking
    QOAuthHttpServerReplyHandler* m_replyHandler;
    quint16 m_callbackPort = 8080;

    std::chrono::milliseconds m_tokenExpiryTime;

    int m_timeoutTimerId = 0;
    int m_refreshTimerId = 0;

};