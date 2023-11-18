// Copyright (C) 2023 akiaji-k

#ifndef PROCESS_STEP_WIDGET_H
#define PROCESS_STEP_WIDGET_H

#include <QWidget>
#include <QFrame>
#include <QSpacerItem>

namespace Ui {
class ProcessStepWidget;
}

class ProcessStepWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ProcessStepWidget(QWidget *parent = nullptr);
    ~ProcessStepWidget();

    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::ProcessStepWidget *m_ui;

    size_t m_index = 0;
    QString m_border_size = "3px";
    QString m_border_style = "solid";
    QString m_border_color = "rgb(255, 0, 0)";

public:
    void SetIndex(size_t index);
    void SetExplanation(QString explanation);
    void SetBorderSize(QString size)
    {
        m_border_size = size;
        return;
    }
    void SetBorderStyle(QString style)
    {
        m_border_style = style;
        return;
    }
    void SetBorderColor(QString color)
    {
        m_border_color = color;
        return;
    }
    void Activate(bool status);
    void ActivateBottomRightWidget(bool status);

signals:
    void clicked_signal();
};

#endif // PROCESS_STEP_WIDGET_H
