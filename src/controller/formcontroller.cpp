#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVector>
#include <QDebug>
#include <QImage>
#include <QDateTime>
#include "formcontroller.h"
#include "httpsessionstore.h"

extern HttpSessionStore* sessionStore;

FormController::FormController()
{}

void FormController::service(HttpRequest& request, HttpResponse& response)
{
    int count = 0;
    int count2 = 0;
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    QFile users("etc/docroot/users.txt");
    QVector<QString> errors;
    qDebug() << request.getUploadedFile("file_sprint");
    if (request.getMethod() == "POST")
    {
        if (request.getParameter("F") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("I") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("O") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("email") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("birthday") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("login") == "")
        {
            errors.append("Заполните пожалуйста поле");
        }
        else
        {
            errors.append("");
        }
        if (request.getParameter("password") != request.getParameter("password_confirmation"))
        {
            errors.append("Пароли не совпадают");
        }
        else
        {
            errors.append("");
        }
        for (int i = 0; i < errors.length(); i++)
        {
            if (errors.at(i) != "")
            {
                count++;
                count2++;
            }
        }

        if (count == 0)
        {
            int line_count=0;
            if (users.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&users);
                while (!in.atEnd()) {
                    in.readLine();
                    line_count++;
                }
            }
            users.close();
            users.open(QIODevice::WriteOnly|QIODevice::Append);
            users.write(QString::number(line_count+1).toUtf8() + " ");
            users.write(request.getParameter("F")+" ");
            users.write(request.getParameter("I")+" ");
            users.write(request.getParameter("O")+" ");
            users.write(request.getParameter("email")+" ");
            users.write(request.getParameter("birthday")+" ");
            users.write(request.getParameter("login")+" ");
            QString pw = QString(QCryptographicHash::hash(request.getParameter("password"),QCryptographicHash::Md5).toHex());
            users.write(pw.toUtf8()+" ");
            users.write(QString::number(request.getParameter("city").toInt()).toUtf8()+" ");
            QString current = QDateTime::currentDateTime().toString("yyyy-MM-dd|hh:mm:ss:zzz");
            users.write(current.toUtf8()+" ");
            users.write("1 ");
            users.write("1\n");
            users.close();
            QImage im;
            QByteArray ba;
            QBuffer buffer(&ba);
            im.save(request.getUploadedFile("avatar"));
            QTemporaryFile* file=request.getUploadedFile("avatar");
            if (file)
            {
                while (!file->atEnd() && !file->error())
                {
                    ba.append(file->read(65536));
                }
            }
            buffer.open(QIODevice::WriteOnly);
            im.save(&buffer, "PNG");
            QFile res("etc/docroot/"+request.getParameter("login")+".png");
            res.open(QIODevice::WriteOnly);
            res.write(ba);
            res.close();
        }
    }
    else
    {
        errors = {"","","","","","",""};
    }
    QByteArray page;
    page.append("<html lang=\"ru\" data-theme=\"light\">");
    page.append("<head>");
    page.append("<meta charset=\"UTF-8\">");
    page.append("<title>Registration</title>");
    page.append("<link rel=\"stylesheet\" href=\"style.css\">");
    page.append("</head>");

    page.append("<body>");
    page.append("<form class=\"card\" action=\"registration\" method=\"post\" enctype=\"multipart/form-data\">");
    page.append("<h2>Регистрация</h2>");

    page.append("<div class=\"login__content\">");

    page.append("<label for=\"F\">");
    page.append("Фамилия");
    if (count2 != 0)
    {
        page.append("<input type=\"text\" id=\"F\" name=\"F\" value=\""+request.getParameter("F")+"\">");
    }
    else
    {
        page.append("<input type=\"text\" id=\"F\" name=\"F\">");
    }
    page.append("<small>"+errors.at(0)+"</small>");
    page.append("</label>");

    page.append("<label for=\"I\">");
    page.append("Имя");
    if (count2 != 0)
    {
        page.append("<input type=\"text\" id=\"I\" name=\"I\" value=\""+request.getParameter("I")+"\">");
    }
    else
    {
        page.append("<input type=\"text\" id=\"I\" name=\"I\">");
    }
    page.append("<small>"+errors.at(1)+"</small>");
    page.append("</label>");

    page.append("<label for=\"O\">");
    page.append("Отчество");
    if (count2 != 0)
    {
        page.append("<input type=\"text\" id=\"O\" name=\"O\" value=\""+request.getParameter("O")+"\">");
    }
    else
    {
        page.append("<input type=\"text\" id=\"O\" name=\"O\">");
    }
    page.append("<small>"+errors.at(2)+"</small>");
    page.append("</label>");

    page.append("<label for=\"Email\">");
    page.append("Email");
    if (count2 != 0)
    {
        page.append("<input type=\"email\" id=\"email\" name=\"email\" value=\""+request.getParameter("email")+"\">");
    }
    else
    {
        page.append("<input type=\"email\" id=\"email\" name=\"email\">");
    }
    page.append("<small>"+errors.at(3)+"</small>");
    page.append("</label>");

    page.append("<label for=\"Birthday\">");
    page.append("Дата рождения");
    if (count2 != 0)
    {
        page.append("<input type=\"date\" id=\"birthday\" name=\"birthday\" value=\""+request.getParameter("birthday")+"\">");
    }
    else
    {
        page.append("<input type=\"date\" id=\"birthday\" name=\"birthday\">");
    }
    page.append("<small>"+errors.at(4)+"</small>");
    page.append("</label>");

    page.append("<label for=\"Login\">");
    page.append("Логин");
    if (count2 != 0)
    {
        page.append("<input type=\"text\" id=\"login\" name=\"login\" value=\""+request.getParameter("login")+"\">");
    }
    else
    {
        page.append("<input type=\"text\" id=\"login\" name=\"login\">");
    }
    page.append("<small>"+errors.at(5)+"</small>");
    page.append("</label>");

    page.append("<div class=\"grid\">");

    page.append("<label for=\"password\">");
    page.append("Пароль");
    if (count2 != 0)
    {
        page.append("<input type=\"password\" id=\"password\" name=\"password\" value=\""+request.getParameter("password")+"\">");
    }
    else
    {
        page.append("<input type=\"password\" id=\"password\" name=\"password\">");
    }
    page.append("<small></small>");
    page.append("</label>");

    page.append("<label for=\"password_confirmation\">");
    page.append("Подтверждение");
    page.append("<input type=\"password\" id=\"password_confirmation\" name=\"password_confirmation\">");
    page.append("<small>"+errors.at(6)+"</small>");
    page.append("</label>");

    page.append("</div>");

    page.append("<label for=\"City\">");
    page.append("Выберите город");
    page.append("<select name=\"city\" id=\"city\" class = \"city\"/>");
    QFile cities("etc/docroot/cities.txt");
    cities.open(QIODevice::ReadOnly);
    QTextStream in(&cities);
    in.setCodec("UTF-8");
    while(!in.atEnd())
    {
        QString line = in.readLine();
        QStringList list = line.split(QRegularExpression("\\s+"));
        page.append("<option value=\""+list.at(0)+"\">"+list.at(1).toUtf8()+"</option>");
    }
    cities.close();
    page.append("</select>");
    page.append("<small></small>");
    page.append("</label>");

    page.append("<label for=\"avatar\">");
    page.append("Добавьте аватарку");
    page.append("<input type=\"file\" id=\"avatar\" name=\"avatar\" accept=\".png, .jpeg, .jpg, .bmp\">");
    page.append("<small></small>");
    page.append("</label>");

    page.append("<button type=\"submit\" id=\"submit\" class=\"login__submit\">Зарегистрироваться</button>");

    page.append("</div>");

    page.append("<p class=\"login__p\">У меня уже есть <a class=\"login__reg\" href=\"login\">аккаунт</a></p>");

    page.append("</form>");

    page.append("</body>");



    response.write(page,true);
}

