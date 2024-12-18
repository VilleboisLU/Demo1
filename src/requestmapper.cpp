/**
  @file
  @author Stefan Frings
*/

#include <QCoreApplication>
#include "global.h"
#include "requestmapper.h"
#include "filelogger.h"
#include "staticfilecontroller.h"
#include "controller/dumpcontroller.h"
#include "controller/templatecontroller.h"
#include "controller/formcontroller.h"
#include "controller/fileuploadcontroller.h"
#include "controller/sessioncontroller.h"
#include "controller/logincontroller.h"
#include "httpsessionstore.h"

extern HttpSessionStore* sessionStore;

RequestMapper::RequestMapper(QObject* parent)
    :HttpRequestHandler(parent)
{
    qDebug("RequestMapper: created");
}


RequestMapper::~RequestMapper()
{
    qDebug("RequestMapper: deleted");
}


void RequestMapper::service(HttpRequest& request, HttpResponse& response)
{
    QByteArray path=request.getPath();
    qDebug("RequestMapper: path=%s",path.data());

    HttpSession session = sessionStore->getSession(request, response);
    //qDebug() << session.getId();
            // For the following pathes, each request gets its own new instance of the related controller.

    if (path.startsWith("/main"))
    {        
        DumpController().service(request, response);
    }

    else if (path.startsWith("/messages"))
    {
        TemplateController().service(request, response);
    }

    else if (path.startsWith("/registration"))
    {
        FormController().service(request, response);
    }

    else if (path.startsWith("/news"))
    {
        FileUploadController().service(request, response);
    }

    else if (path.startsWith("/profile"))
    {
        SessionController().service(request, response);
    }

    else if (path.startsWith("/login"))
    {
        LoginController().service(request, response);
    }
    else
    {
        staticFileController->service(request, response);
    }

    qDebug("RequestMapper: finished request");

    // Clear the log buffer
    if (logger)
    {
       logger->clear();
    }
}
