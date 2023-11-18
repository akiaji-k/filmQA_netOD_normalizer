// Copyright (C) 2023 akiaji-k

#include "process_step_widget.h"
#include "ui_process_step_widget.h"

ProcessStepWidget::ProcessStepWidget(QWidget *parent) :
    QFrame(parent),
    m_ui(new Ui::ProcessStepWidget)
{
    m_ui->setupUi(this);
}

ProcessStepWidget::~ProcessStepWidget()
{
    delete m_ui;
}

void ProcessStepWidget::SetIndex(size_t index)
{
    m_index = index;
    m_ui->indexLabel->setText(QString::number(index) + ".");
    return;
}

void ProcessStepWidget::SetExplanation(QString explanation)
{
    m_ui->explainLabel->setText(explanation);
    return;
}

void ProcessStepWidget::Activate(bool status)
{
//    this->setEnabled(status);
    this->setEnabled(true);

    m_ui->indexLabel->setStyleSheet("");
    m_ui->explainLabel->setStyleSheet("");
    this->setStyleSheet("");


    if (status) {
        ActivateBottomRightWidget(status);
        m_ui->indexLabel->setStyleSheet("#indexLabel{ color: black; }");
        m_ui->explainLabel->setStyleSheet("#explainLabel{ color: black; }");

        this->setStyleSheet(QString("#processStepWidget_%1{\
                                                border-style: none;\
                                                border-left: %2 %3 %4;\
                                             }").arg(QString::number(m_index)).arg(m_border_size).arg(m_border_style).arg(m_border_color));
    }
    else {
        ActivateBottomRightWidget(status);
        m_ui->indexLabel->setStyleSheet("#indexLabel{ color: gray; }");
        m_ui->explainLabel->setStyleSheet("#explainLabel{ color: gray; }");

        this->setStyleSheet(QString("#processStepWidget_%1{\
                                                border-style: none;\
                                                border-left: %2 %3 gray;\
                                             }").arg(QString::number(m_index)).arg(m_border_size).arg(m_border_style));

    }

    return;
}

void ProcessStepWidget::ActivateBottomRightWidget(bool status)
{
    auto item = static_cast<QGridLayout*>(this->layout())->itemAtPosition(1, 1);

    if (item != nullptr) {
        auto widget = item->widget();
        auto layout = item->layout();

        if (widget != nullptr) {
//            qDebug() << "widget is not a nullptr";
            widget->setEnabled(status);
        }
        else if (layout != nullptr) {
//            qDebug() << "layout is not a nullptr";
            for (int i = 0; i < layout->count(); ++i) {
                auto widget_in_layout = layout->itemAt(i)->widget();
                if (widget_in_layout != nullptr) {
                    widget_in_layout->setEnabled(status);
                }
                else {;}
            }
            layout->setEnabled(status);
        }
        else {;}
    }
    else {;}

    return;
}




void ProcessStepWidget::mousePressEvent(QMouseEvent *event)
{
//    qDebug() << "ProcessStepWidget::mousePressEvent(QMouseEvent *event) is called.";
    emit clicked_signal();

    return;
}

