#pragma once

#include <iostream>
#include <QOAuth2AuthorizationCodeFlow>
#include <QOAuthHttpServerReplyHandler>
#include <QUrl>
#include <QString>

class GoogleSSO : public QOAuth2AuthorizationCodeFlow {

    Q_OBJECT

public:
    GoogleSSO(const QString&, QObject*);
    virtual ~GoogleSSO();

    QString clientId() const;
    void setClientId(const QString&);

    QString clientSecret() const;
    void setClientSecret(const QString&);

    void removeSavedTokens();

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
    QOAuthHttpServerReplyHandler* m_replyHandler{ new QOAuthHttpServerReplyHandler{m_callbackPort, this} };
    quint16 m_callbackPort = 8080;

    // Timers
    std::chrono::milliseconds m_tokenExpiryTime;
    int m_timeoutTimerId = 0;
    int m_refreshTimerId = 0;

    // Storing tokens
    void _save_tokens();
    void _load_tokens();

    // Setup sso configuration
    inline void _configure_sso(const QString&);

    // Setup signals and slots
    inline void _configure_signals_and_slots();

};