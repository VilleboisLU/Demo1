#include <QDateTime>
#include "../global.h"
#include "sessioncontroller.h"
#include "httpsessionstore.h"
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

SessionController::SessionController()
{}

void SessionController::service(HttpRequest& request, HttpResponse& response)
{

    response.setHeader("Content-Type", "text/html; charset=UTF-8");

    // Get current session, or create a new one
    HttpSession session=sessionStore->getSession(request,response);

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

    if (request.getParameter("id") == "")
    {
        page.append("<article>");
        page.append("<div class=\"profile\">");
        page.append("<div class="">");
        page.append("<img class=\"profile__avatar\" src=\""+user.at(6)+".png\" alt=\"avatar\">");
        page.append("</div>");
        page.append("<div class=\"user__info\">");
        page.append("<p><strong>"+user.at(1)+" "+user.at(2)+" "+user.at(3)+"</strong></p>");
        page.append("<p><strong>"+user.at(4)+"</strong></p>");
        page.append("</div>");
        page.append("</div>");
        page.append("</article>");
    }
    else
    {
        if (!users.open(QIODevice::ReadOnly))
        {
            return ;
        }
        in.setCodec("Utf-8");
        QVector<QStringList> tableUsers = {};
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(QRegularExpression("\\s+"));
            tableUsers.append(list);
        }
        users.close();
        page.append("<article>");
        page.append("<div class=\"profile\">");
        page.append("<div class="">");
        page.append("<img class=\"profile__avatar\" src=\""+tableUsers.at(request.getParameter("id").toInt()-1).at(6)+".png\" alt=\"avatar\">");
        page.append("</div>");
        page.append("<div class=\"user__info\">");
        page.append("<p><strong>"+tableUsers.at(request.getParameter("id").toInt()-1).at(1)+" "+tableUsers.at(request.getParameter("id").toInt()-1).at(2)+" "+tableUsers.at(request.getParameter("id").toInt()-1).at(3)+"</strong></p>");
        page.append("<p><strong>"+tableUsers.at(request.getParameter("id").toInt()-1).at(4)+"</strong></p>");
        page.append("</div>");
        page.append("</div>");
        page.append("</article>");
    }

    page.append("</main>");

    page.append("</body>");

    response.write(page,true);
}
