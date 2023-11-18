// Copyright (C) 2023 akiaji-k

#include "image_path_edit.h"

#include <QMimeData>
#include <QFileDialog>
#include <QDragEnterEvent>
#include <QDebug>


ImagePathEdit::ImagePathEdit(QWidget *parent) :
//    QPlainTextEdit(parent)
    QTextEdit(parent)
{
    // set ReadOnly to avoid unintended manipulation of the input path.
    setReadOnly(true);

    SetHowToUse();

    return;
}

void ImagePathEdit::ResetToInitial()
{
    qDebug() << "void ImagePathEdit::reset_to_initial() is called.";

    m_input_urls.clear();
    SetHowToUse();

    return;
}


void ImagePathEdit::dragEnterEvent(QDragEnterEvent *e)
{
    // unset ReadOnly to enable drop item
    if(isReadOnly()) {
        setReadOnly(false);
    }

    e->acceptProposedAction();

    return;
}


void ImagePathEdit::dropEvent(QDropEvent *e)
{
    m_input_urls.clear();
    m_input_urls = e->mimeData()->urls();

    DisplayInputs();

    // set ReadOnly to avoid unintended manipulation of the input path.
    if(!isReadOnly()) {
        setReadOnly(true);
    }

    return;
}


void ImagePathEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    m_input_urls.clear();
    m_input_urls = QFileDialog::getOpenFileUrls();

    DisplayInputs();

    return;
}

void ImagePathEdit::DisplayInputs()
{
    this->clear();

    if (PathValidation() == true) {
        this->setTextColor(Qt::black);
//        this->setAlignment(Qt::AlignLeft);

        QString inputs;
        for (auto&& elem : m_input_urls) {
            inputs += elem.toLocalFile();
            inputs += "\n";
        }

        setPlainText(inputs);
    //    qDebug() << "Input file path:" << toPlainText();
        for (auto&& elem : m_input_urls) {
            qDebug() << "file:" << elem;
        }

        emit valid_image_path_signal(m_input_urls);
    } else {
        SetHowToUse();
    }

    return;
}

bool ImagePathEdit::PathValidation()
{
    bool ret = false;

    for(auto&& elem : m_input_urls) {
        fs::path input = elem.toLocalFile().toStdString();

        if (fs::is_regular_file(input) == false
                || m_image_extensions.contains(input.extension()) == false) {

            qDebug() << "ERROR: The input files contain non-image file.";
            ret = false;
            break;
        }
        else {
            ret = true;
        }
    }

    return ret;
}

void ImagePathEdit::SetHowToUse()
{
    this->document()->clear();

    if(this->document()->isEmpty()){
        this->setTextColor(Qt::darkGray);
//        this->setText(m_how_to_use);
        this->setText(tr("drag & drop input items or double-click to select input items."));

//        this->setAlignment(Qt::AlignCenter);
    }
    else {
        this->setTextColor(Qt::black);
//        this->setAlignment(Qt::AlignLeft);
    }

    return;
}

