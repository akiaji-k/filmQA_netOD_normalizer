// Copyright (C) 2023 akiaji-k

#include "pushbutton_with_word_wrap.h"

PushButtonWithWordWrap::PushButtonWithWordWrap(QWidget *parent)
    : QPushButton{parent}
{
    QSizePolicy policy = QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setSizePolicy(policy);
    m_label->setWordWrap(true);
    m_hbox_layout->addWidget(m_label.get());

}

void PushButtonWithWordWrap::SetText(const QString &text)
{
    m_label->setText(text);

    return;
}
