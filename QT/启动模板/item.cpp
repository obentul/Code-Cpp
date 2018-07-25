#include <QGuiApplication>
#include <QQuickView>

int main(int argc,char *argv[])
{
	QGuiApplication app(argc,char argv);

	QQuickView viewer;
	viewer.setResizeMode(QQuickView::SizeRootObjectToView);
	viewer.setSource(Qurl("qrc:///main.sqml"));
	viewer.show();

	return app.exec();
}
