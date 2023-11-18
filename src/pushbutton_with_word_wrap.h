// Copyright (C) 2023 akiaji-k

#ifndef PUSHBUTTONWITHWORDWRAP_H
#define PUSHBUTTONWITHWORDWRAP_H

#include <QHBoxLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <memory>

class PushButtonWithWordWrap : public QPushButton
{
    Q_OBJECT
public:
    explicit PushButtonWithWordWrap(QWidget *parent = nullptr);

    void SetText(const QString &text);

private:
    std::unique_ptr<QLabel> m_label = std::make_unique<QLabel>("", this);
    std::unique_ptr<QHBoxLayout> m_hbox_layout = std::make_unique<QHBoxLayout>(this);
};

#endif // PUSHBUTTONWITHWORDWRAP_H
