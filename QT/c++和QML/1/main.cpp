/* 导入QML类型需要2步：1）在响应的qml中import
 *                     2）在main函数中qmlRegisterType
 * 使用实例只需要1步：1）在main函数中setContextProperty
 *
 * 但是，使用QML类型更直接和通用，使用实例会有很多棘手的问题需要解决
 *
*/

#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQml>
#include "colorMaker.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    //[1]导入一个QML类型
    //qmlRegisterType<ColorMaker>("an.qt.ColorMaker", 1, 0, "ColorMaker");

    QQuickView viewer;
    QObject::connect(viewer.engine(), SIGNAL(quit()), &app, SLOT(quit()));  
    viewer.setResizeMode(QQuickView::SizeRootObjectToView);
    viewer.rootContext()->setContextProperty("colorMaker", new ColorMaker); //[2]导入一个实例
    viewer.setSource(QUrl("qrc:///main.qml"));
    viewer.show();

    return app.exec();
}
