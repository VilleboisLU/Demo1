#include "fileuploadcontroller.h"
#include "httpsessionstore.h"
#include <QVariant>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QAuthenticator>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QUrlQuery>

extern HttpSessionStore* sessionStore;

FileUploadController::FileUploadController()
{}

void FileUploadController::service(HttpRequest& request, HttpResponse& response)
{

    QEventLoop eventLoop;
    QJsonDocument doc;
    QNetworkReply *reply;
    QByteArray res;

    HttpSession session = sessionStore->getSession(request, response);

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_0);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);

    QFile ca("RootCA.crt");
    if (ca.open(QIODevice::ReadOnly))
    {
      QSslCertificate cert = QSslCertificate(&ca);
      QList <QSslCertificate> caList;
      caList.append(cert);
      sslConfig.setCaCertificates(caList);
    }

    netMan = new QNetworkAccessManager();
    QObject::connect(netMan, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest reqEmb;
    reqEmb.setUrl(QUrl( QString("https://api.currentsapi.services/v1/latest-news?language=ru&apiKey=2EKj5_lh_5GDEYHF3OPeWJp70eK2gulCD_f6IaGA-FQWUd-R") ));
    reqEmb.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    reqEmb.setSslConfiguration(sslConfig);

    reply = netMan->get(reqEmb);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError){
        res = reply->readAll();
    }

    netMan->disconnect();

    QJsonDocument result;
    QJsonObject obj;
    QJsonValue val;
    QJsonArray arr;
    qDebug() << res;
    result = QJsonDocument::fromJson(res);
    obj = result.object();
    val = obj.value("news");
    arr = val.toArray();

    QFile users("etc/docroot/users.txt");
    if (!users.open(QIODevice::ReadOnly))
    {
        return ;
    }
    QTextStream in(&users);
    in.setCodec("Utf-8");
    QStringList user = {};
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList list = line.split(QRegularExpression("\\s+"));
        if (session.get("user_id").toInt() == list.at(0).toInt())
        {
            user = list;
        }
    }
    users.close();

    QByteArray page;
    page.append("<html lang=\"ru\" data-theme=\"light\">");
    page.append("<head>");
    page.append("<meta charset=\"UTF-8\">");
    page.append("<title>Login</title>");
    page.append("<link rel=\"stylesheet\" href=\"home.css\">");
    page.append("<link rel=\"stylesheet\" href=\"header.css\">");
    page.append("</head>");

    page.append("<body>");

    page.append("<header class=\"header\">");
    page.append("<div class=\"header__user\">");
    page.append("<a href=\"./session\">");
    page.append("<img class=\"avatar\" src = \""+session.get("login").toString()+".png\"  alt=\"\" >");
    page.append("</a>");
    page.append("<div>");
    page.append("<h3 class=\"profile-name\"><a href=\"/profile\">"+user.at(1).toUtf8()+" "+user.at(2).toUtf8()+" "+user.at(3).toUtf8()+" "+"</a></h3>");
    page.append("<span>"+session.get("email").toString().toUtf8()+"</span>");
    page.append("</div>");
    page.append("</div>");

    page.append("<form class=\"logout\" method=\"post\" action=\"/login?action=logout\">");
    page.append("<button role=\"button\">Выйти</button>");
    page.append("</form>");
    page.append("</header>");

    page.append("<main>");

    page.append("<nav class=\"navigation\">");
    page.append("<ul class=\"navigation__list\">");
    page.append("<li>");
    page.append("<a href=\"/news\">Новости</a>");
    page.append("</li>");
    page.append("<li>");
    page.append("<a href=\"/messages\">Сообщения</a>");
    page.append("</li>");
    page.append("<li>");
    page.append("<a href=\"/main\">Пользователи</a>");
    page.append("</li>");
    page.append("</ul>");
    page.append("</nav>");

    page.append("<article class=\"news\">");
    int t = 0;
    for (const QJsonValue & value : arr)
    {
        page.append("<div class=\"content\">");
        page.append("<h1>"+value["title"].toString()+"</h1>");
        page.append("<img class=\"news__image\" src=\""+value["image"].toString()+"\"/>");
        page.append("<p class=\"news__content\"><a href=\""+value["url"].toString()+"\">"+value["description"].toString()+"</a></p>");
        page.append("</div>");
        if (t>3)
        {
            break;
        }
        t++;
    }

    page.append("</article>");
    page.append("</main>");

    page.append("</body>");

    response.write(page,true);
}

