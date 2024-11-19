#include "googlesso.h"
#include <QDesktopServices>
#include <QTimerEvent>
#include <QTimer>

#include <QSettings>

GoogleSSO::GoogleSSO(const QString& scope, QObject* parent)
    : QOAuth2AuthorizationCodeFlow(parent)
    , m_replyHandler{ new QOAuthHttpServerReplyHandler{m_callbackPort, this} }
{

    // Namespaces
    using namespace std::chrono_literals;

    QSettings settings{ DEVELOPMENT_PATH"/secret.ini", QSettings::Format::IniFormat };

    QString clientId = settings.value("clientId").toString();
    QString clientSecret = settings.value("clientSecret").toString();

    qInfo() << clientId;
    qInfo() << clientSecret;

    // Configuring the authorization flow
    this->setClientIdentifier(clientId);
    this->setClientIdentifierSharedKey(clientSecret);
    this->setScope(scope);
    this->setAuthorizationUrl(AUTHORIZATION_CODE_URI);
    this->setAccessTokenUrl(ACCESS_TOKEN_URI);
    this->setPkceMethod(QOAuth2AuthorizationCodeFlow::PkceMethod::S256);
    this->setReplyHandler(m_replyHandler);

    this->loadTokens();

    // Connecting signals to slots
    connect(this, &QAbstractOAuth::authorizeWithBrowser, this, [this](const QUrl& url) {

        if (QDesktopServices::openUrl(url))
            // Setup the authorization timeout timer
            this->m_timeoutTimerId = this->startTimer(60s);

        });

    connect(this, &QAbstractOAuth::granted, this, [this]() {

        // Close the server
        if (m_replyHandler->isListening())
            this->m_replyHandler->close();

        // Set the token refresh timer
        m_tokenExpiryTime = (this->expirationAt() - QDateTime::currentDateTime());
        m_refreshTimerId = this->startTimer(m_tokenExpiryTime - 60s);

        // Kill the authorization timeout timer
        killTimer(m_timeoutTimerId);
        });

    connect(this, &QOAuth2AuthorizationCodeFlow::statusChanged,
        this, &GoogleSSO::onStatusChanged);

    connect(this, &QAbstractOAuth::requestFailed, this, &GoogleSSO::onRequestFailed);

    // Setup the token refresher timer. Refresh access token each hour
}
GoogleSSO::~GoogleSSO()
{
    this->saveTokens();
}

const QUrl GoogleSSO::AUTHORIZATION_CODE_URI = QUrl{ "https://accounts.google.com/o/oauth2/auth" };
const QUrl GoogleSSO::ACCESS_TOKEN_URI = QUrl{ "https://oauth2.googleapis.com/token" };

QString GoogleSSO::clientId() const
{
    return this->clientIdentifier();
}

void GoogleSSO::setClientId(const QString& clientId)
{
    this->setClientIdentifier(clientId);
}

QString GoogleSSO::clientSecret() const
{
    return this->clientIdentifierSharedKey();
}

void GoogleSSO::setClientSecret(const QString& clientSecret)
{
    this->setClientIdentifierSharedKey(clientSecret);
}

void GoogleSSO::saveTokens()
{
    QSettings settings;

    settings.beginGroup("tokens");

    settings.setValue("access_token", this->token());
    settings.setValue("refresh_token", this->refreshToken());

    settings.endGroup();

}

void GoogleSSO::loadTokens()
{
    QSettings settings;

    settings.beginGroup("tokens");

    QString refreshToken = settings.value("refresh_token").toString();

    if (refreshToken.isEmpty())
        return;

    this->setRefreshToken(refreshToken);

    settings.endGroup();

    // Refresh the access token
    this->refreshAccessToken();
}

void GoogleSSO::onRequestFailed(QAbstractOAuth::Error error)
{
    
    qInfo().noquote() << "Token request failed:";

    switch (error)
    {
    case QAbstractOAuth::Error::NoError:
        qInfo() << "No error.";
        break;
    case QAbstractOAuth::Error::NetworkError:
        qInfo() << "Network error.";
        break;
    case QAbstractOAuth::Error::ServerError:
        qInfo() << "Server error.";
        break;
    case QAbstractOAuth::Error::OAuthCallbackNotVerified:
        qInfo() << "Callback not verified.";
        break;
    case QAbstractOAuth::Error::OAuthTokenNotFoundError:
        qInfo() << "Authorization token not found.";
        break;
    case QAbstractOAuth::Error::OAuthTokenSecretNotFoundError:
        qInfo() << "Authorization token secret (client secret) not found.";
        break;
    default:
        break;
    }

}

void GoogleSSO::onStatusChanged(QAbstractOAuth::Status status)
{

    QDebug info = qInfo().nospace().noquote();

    info << "Status of the authentication process changed. Current status: ";
    
    switch (status)
    {
    case QAbstractOAuth::Status::NotAuthenticated:
        info << "Not authenticated.";
        break;

    case QAbstractOAuth::Status::Granted:
        info << "Authentication granted.";
        break;

    case QAbstractOAuth::Status::TemporaryCredentialsReceived:
        info << "Temporary credentials received.";
        break;

    case QAbstractOAuth::Status::RefreshingToken:
        info << "Refreshing token.";
        break;

    default:
        break;
    }

}

void GoogleSSO::timerEvent(QTimerEvent* event)
{

    if (event->timerId() == m_refreshTimerId) {
        this->refreshAccessToken();
        return;
    }
    else if (event->timerId() == m_timeoutTimerId) {

        qInfo() << "Timer fired.";

        if (this->status() != QAbstractOAuth::Status::Granted) {
            qDebug() << "Authentication timed out.";
            //this->m_replyHandler->close();

        }

    }



}

void GoogleSSO::authenticate() {

    QDebug critical = qCritical().noquote().nospace();

    if (!this->m_replyHandler->isListening()) {
        critical << "Server not listening on port: " << m_callbackPort << ".";
        return;
    }

    if (this->clientId().isEmpty()) {
        critical << "No client ID provided.";
        return;
    }

    if (this->clientSecret().isEmpty()) {
        critical << "No client secret provided.";
        return;
    }

    this->grant(); // Start the authorization process
}