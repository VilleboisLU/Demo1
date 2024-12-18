#include "dumpcontroller.h"
#include <QVariant>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QThread>
#include "httpsessionstore.h"

extern HttpSessionStore* sessionStore;

DumpController::DumpController()
{}

void DumpController::service(HttpRequest& request, HttpResponse& response)
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

    page.append("<article class=\"users\">");
    page.append("<table class=\"users__table\">");
    page.append("<tr><th>ID</th><th>Avatar</th><th>FIO</th><th>Email</th><th>Birthday</th><th>Login</th><th>Password</th><th>Reg. Date</th><th>City</th></tr>");
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
        page.append("<tr>");
        page.append("<td>"+tableUsers.at(i).at(0)+"</td>");
        page.append("<td><img class=\"avatar\" src=\""+tableUsers.at(i).at(6)+".png\" alt=\"ava.png\"></td>");
        page.append("<td><a href=\"/profile?id="+tableUsers.at(i).at(0)+"\">"+tableUsers.at(i).at(1)+" "+tableUsers.at(i).at(2)+" "+tableUsers.at(i).at(3)+"</a></td>");
        page.append("<td>"+tableUsers.at(i).at(4)+"</td>");
        page.append("<td>"+tableUsers.at(i).at(5)+"</td>");
        page.append("<td>"+tableUsers.at(i).at(6)+"</td>");
        page.append("<td>"+tableUsers.at(i).at(7)+"</td>");
        page.append("<td>"+tableUsers.at(i).at(9)+"</td>");
        page.append("<td><a href=\"https://yandex.ru/maps/?ll="+tableCities.at(tableUsers.at(i).at(8).toInt()-1).at(2)+"%2C"+tableCities.at(tableUsers.at(i).at(8).toInt()-1).at(3)+"\">"+tableCities.at(tableUsers.at(i).at(8).toInt()-1).at(1)+"</a></td>");
        page.append("</tr>");
    }

    page.append("</table>");
    page.append("</article>");
    page.append("</main>");

    page.append("</body>");

    response.write(page,true);
}
