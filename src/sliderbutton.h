#ifndef SLIDERBUTTON_H
#define SLIDERBUTTON_H

#include <QWidget>
#include <QPushButton>

class SliderButton : public QWidget
{
    Q_OBJECT
public:
    explicit SliderButton(const QString &offParameter, const QString &onParameter, QWidget *parent = nullptr);
    ~SliderButton();

private:

    QPushButton* btn1;
    QPushButton* btn2;
    bool stateSlider;
    int width1;
    int width2;
    int height;
    QString label1;
    QString label2;

private slots:

    void changeStateSlider();

signals:

    void currentState(bool);

public slots:

};

#endif // SLIDERBUTTON_H
