#include "../global.h"
#include "templatecontroller.h"
#include "templatecache.h"
#include "template.h"
#include "httpsessionstore.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>

extern HttpSessionStore* sessionStore;

TemplateController::TemplateController()
{}

void TemplateController::service(HttpRequest& request, HttpResponse& response)
{
    HttpSession session = sessionStore->getSession(request, response);
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
    page.append("<h3 class=\"profile-name\">"+user.at(1).toUtf8()+" "+user.at(2).toUtf8()+" "+user.at(3).toUtf8()+" "+"</h3>");
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

    page.append("<article class=\"messages\">");
    page.append("<div class=\"messages__table\">");
    if (request.getMethod() == "GET" && request.getParameter("id")!="")
    {
        QFile messages("etc/docroot/messages.txt");
        if (!messages.open(QIODevice::ReadOnly))
        {
            return ;
        }
        in.setCodec("Utf-8");
        QVector<QStringList> tableMessages = {};
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(QRegularExpression("\\s+"));
            tableMessages.append(list);
        }
        messages.close();
        page.append("<article class=\"messages\">");
        page.append("<div class=\"messages__table\">");



        page.append("</div>");
        page.append("</article>");

        page.append("</body>");

        response.write(page,true);
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
    QVector<QStringList> tableCities = {};
    QFile cities("etc/docroot/cities.txt");
    cities.open(QIODevice::ReadOnly);
    QTextStream inc(&cities);
    inc.setCodec("UTF-8");
    while(!inc.atEnd())
    {
        QString line = inc.readLine();
        QStringList list = line.split(QRegularExpression("\\s+"));
        tableCities.append(list);
    }
    cities.close();

    for(int i = 0; i < tableUsers.length(); i++)
    {
        if(session.get("user_id").toInt()!=tableUsers.at(i).at(0).toInt())
        {
            page.append("<a href=\"/messages?id="+QString::number(tableUsers.at(i).at(0).toInt())+"\">");
            page.append("<div class=\"messages__row\">");
            page.append("<img class=\"avatar\" src=\""+tableUsers.at(i).at(6)+".png\" alt=\"ava.png\">");
            page.append(tableUsers.at(i).at(1)+" "+tableUsers.at(i).at(2)+" "+tableUsers.at(i).at(3));
            page.append("</div>");
            page.append("</a>");
        }
    }
    page.append("</div>");

    page.append("</article>");
    page.append("</main>");

    page.append("</body>");

    response.write(page,true);
    }
}
