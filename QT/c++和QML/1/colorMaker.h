#ifndef COLORMAKER_H
#define COLORMAKER_H
#include <QObject>
#include <QColor>

//继承自QObject
class ColorMaker:public QObject{

    //声明时一个Qt类
    Q_OBJECT
    //枚举类型声明
    Q_ENUMS(GenerateAlgorithm)
    //成员变量声明
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor timeColor READ timeColor)

//外部成员变量和方法
public:
    //没被导出的成员方法(无法在QML中使用)
    ColorMaker(QObject *parent = 0);        //构造函数，为什么*parent=0？
    ~ColorMaker();                          //析构函数
    QColor color() const;                   //成员方法,不修改成员变量
    QColor timeColor() const;               //成员方法，不修改成员变量
    void setColor(const QColor & color);    //入参是常量，不能改变，这里加&是为了提高效率，不是为了修改入参
    //被导出的成员方法(可以在QML中使用)
    Q_INVOKABLE GenerateAlgorithm algorithm() const;    //不修改成员变量，返回GenerateAlgorithm对象实例
    Q_INVOKABLE void setAlgorithm(GenerateAlgorithm algorithm);     //接收1一个GenerateAlgorithm
    //枚举类型的定义
    enum GenerateAlgorithm{
        RandomRGB,
        RandomRed,
        RandomGreen,
        RandomBlue,
        LinearIncrease
    };

//对外开放哪些钩子
signals:
    void colorChanged(const QColor & color);    //可以使用  onColorChanged:{}  和  colorChanged.connect(func)  来为 钩子挂载  处理函数  ，  可以通过 emit 来触发钩子
                                                //入参不可修改，加&只是为了提高效率
    void currentTime(const QString & strTime);  //同上

//槽函数，需要在cpp文件中实现，也可以在新建的类中进行覆盖之类的动作
//虽然这里是槽函数，但是和普通函数是一样的
//最早的时候，signal钩子只能connect槽函数，QT5之后，signal.connect已经可以挂普通函数了，所以在QT中槽函数和普通函数没区别
//在c++中，还是要区分槽函数和普通函数的，因为在c++中，signal还是只能挂槽函数，不能挂普通函数
public slots:
    void start();
    void stop();

//继承属性的成员方法
protected:
    void timerEvent(QTimerEvent *e);

//私有成员方法和变量
private:
    GenerateAlgorithm m_algorithm;
    QColor m_currentColor;
    int m_nColorTimer;
};

#endif // COLORMAKER_H
