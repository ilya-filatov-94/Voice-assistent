#include "sliderbutton.h"

SliderButton::SliderButton(const QString &offParameter, const QString &onParameter, QWidget *parent) : QWidget(parent)
{
    stateSlider = false;
    label1 = offParameter;
    label2 = onParameter;
    btn1 = new QPushButton(offParameter, this);
    btn1->setStyleSheet("text-align: left;");
    connect(btn1, &QPushButton::clicked, this, &SliderButton::changeStateSlider);
    btn1->setObjectName("buttonPartSlider1");
    btn1->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    width1 = label1.size()*12;
    height = btn1->height();

    btn2 = new QPushButton(onParameter, this);
    btn2->setObjectName("buttonPartSlider2");
    btn2->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    width2 = label2.size()*12;

    btn1->setGeometry(0, 0, width1+width2-20, height+4);
    btn2->setGeometry(width1-20, 2, width2, height);

    setMinimumSize(width1+width2-10, height+6);
    setMaximumSize(width1+width2, height+6);
}

SliderButton::~SliderButton()
{
    delete btn1;
    delete btn2;
}

void SliderButton::changeStateSlider()
{
    stateSlider = !stateSlider;
    if (stateSlider) {
        btn2->setGeometry(0, 2, width1, height);
        btn1->setGeometry(0, 0, width1+width2-20, height+4);
        btn1->setText(label2);
        btn1->setStyleSheet("text-align: right;");
        btn2->setText(label1);
        currentState(false);
    }
    else {
        btn1->setGeometry(0, 0, width1+width2-20, height+4);
        btn2->setGeometry(width1-20, 2, width2, height);
        btn1->setText(label1);
        btn1->setStyleSheet("text-align: left;");
        btn2->setText(label2);
        currentState(true);
    }
}
