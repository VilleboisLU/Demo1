#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include "../global.h"
#include "logincontroller.h"
#include "httpsessionstore.h"

extern HttpSessionStore* sessionStore;

LoginController::LoginController()
{}

void LoginController::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    HttpSession session = sessionStore->getSession(request, response);

    if (request.getParameter("action") == "logour")
    {
        session.set("user_id","");
        response.redirect("/login");
    }

    QByteArray auth = request.getHeader("Authorization");
    qDebug() << auth;
    int status = 0;
    QString id = "";
    QStringList user = {};
    QFile users("etc/docroot/users.txt");
    if (!users.open(QIODevice::ReadOnly))
    {
        return ;
    }

    if (request.getParameter("login") != "")
    {
        QTextStream in(&users);
        while(!in.atEnd())
        {
            QString line = in.readLine();
            QStringList list = line.split(QRegularExpression("\\s+"));
            if (list.contains(request.getParameter("login")))
            {
                user = list;
                id = user.at(0);
            }
        }
        if (user.isEmpty())
        {
            status = 1;
        }
        else
        {
            QString pw = QString("%1").arg(QString(QCryptographicHash::hash(request.getParameter("password"),QCryptographicHash::Md5).toHex()));
            if (pw == user.at(7))
            {
                session.set("user_id", id.toInt());
                session.set("login", request.getParameter("login"));
                session.set("FIO", user.at(1).toUtf8()+" "+user.at(2).toUtf8()+" "+user.at(3).toUtf8());
                session.set("email", user.at(4));
                session.set("city", user.at(8).toInt());
                response.redirect("./main");
                return;
            }
            else
            {
                status = 1;
            }
        }
    }

    QByteArray page;
    page.append("<html lang=\"ru\" data-theme=\"light\">");
    page.append("<head>");
    page.append("<meta charset=\"UTF-8\">");
    page.append("<title>Login</title>");
    page.append("<link rel=\"stylesheet\" href=\"style.css\">");
    page.append("</head>");

    page.append("<body>");
    page.append("<form class=\"card\" action=\"./login\" method=\"post\">");
    page.append("<input type=\"hidden\" name=\"redirect\" value=\"./main\"");
    page.append("<h2 class=\"login__header\">Вход</h2>");

    if (status == 1)
    {
        page.append("<div class=\"notice error\">Неверно введен логин или пароль</div>");
    }

    page.append("<div class=\"login__content\">");

    page.append("<label for=\"login\">");
    page.append("Логин");
    page.append("<input type=\"text\" id=\"login\" name=\"login\"");
    page.append("<small></small>");
    page.append("</label>");

    page.append("<label for=\"password\">");
    page.append("Пароль");
    page.append("<input type=\"password\" id=\"password\" name=\"password\"");
    page.append("<small></small>");
    //page.append("<input type=\"hidden\" name=\"redirect\" value=\"/main\"");
    page.append("</label>");

    page.append("<button type=\"submit\" id=\"submit\" class=\"login__submit\" href=\"/main\">Продолжить</button>");

    page.append("</div>");

    page.append("<p class=\"login__p\">У меня еще нет <a class=\"login__reg\" href=\"/registration\">аккаунта</a></p>");

    page.append("</form>");

    page.append("</body>");

    users.close();

    response.write(page,true);
}
