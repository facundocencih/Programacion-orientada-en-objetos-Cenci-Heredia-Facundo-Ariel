#include "codeeditor.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextBlock>

CodeEditor::CodeEditor(QWidget *parent)
    : QTextEdit(parent)
{
    setAcceptRichText(false);
    setLineWrapMode(QTextEdit::NoWrap);
    setFont(QFont("Consolas", 12));
    setTabStopDistance(fontMetrics().horizontalAdvance(' ') * 4);
    connect(this, &QTextEdit::cursorPositionChanged, this, &CodeEditor::detectarCambioDeLinea);
}

void CodeEditor::setValidador(std::unique_ptr<ValidadorSintaxis> validador)
{
    m_validador = std::move(validador);
    m_errores.clear();
    validarLineaActual();
    actualizarResaltado();
}

void CodeEditor::validarLineaActual()
{
    validarLinea(lineaActual());
}

void CodeEditor::validarLinea(int numeroLinea)
{
    if (!m_validador || numeroLinea < 0) {
        return;
    }

    const QTextBlock bloque = bloquePorNumero(numeroLinea);
    if (!bloque.isValid()) {
        return;
    }

    const ResultadoValidacion resultado = m_validador->validarLinea(bloque.text(), numeroLinea + 1);
    if (resultado.valido) {
        m_errores.remove(numeroLinea);
    } else {
        m_errores[numeroLinea] = resultado.diagnostico;
    }

    actualizarResaltado();
    emit lineaValidada(numeroLinea + 1, resultado.valido, resultado.diagnostico);
}

void CodeEditor::focusInEvent(QFocusEvent *event)
{
    emit accionEditor("El editor recibio foco");
    QTextEdit::focusInEvent(event);
}

void CodeEditor::focusOutEvent(QFocusEvent *event)
{
    validarLineaActual();
    emit accionEditor("El editor perdio foco y valido la linea actual");
    QTextEdit::focusOutEvent(event);
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Save)) {
        emit accionEditor("Atajo Ctrl+S detectado en el editor");
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Tab) {
        insertPlainText("    ");
        emit accionEditor("Tabulacion convertida en cuatro espacios");
        return;
    }
    QTextEdit::keyPressEvent(event);
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    emit accionEditor(QString("Click en editor: x=%1 y=%2").arg(event->position().x()).arg(event->position().y()));
    QTextEdit::mousePressEvent(event);
}

void CodeEditor::detectarCambioDeLinea()
{
    const int nuevaLinea = lineaActual();
    if (nuevaLinea != m_ultimaLinea) {
        validarLinea(m_ultimaLinea);
        m_ultimaLinea = nuevaLinea;
        emit accionEditor(QString("Cursor movido a linea %1").arg(nuevaLinea + 1));
    }
}

int CodeEditor::lineaActual() const
{
    return textCursor().blockNumber();
}

QTextBlock CodeEditor::bloquePorNumero(int numeroLinea) const
{
    return document()->findBlockByNumber(numeroLinea);
}

void CodeEditor::actualizarResaltado()
{
    QList<QTextEdit::ExtraSelection> selecciones;

    for (auto it = m_errores.cbegin(); it != m_errores.cend(); ++it) {
        QTextEdit::ExtraSelection seleccion;
        seleccion.format.setBackground(QColor(255, 72, 72, 80));
        seleccion.format.setProperty(QTextFormat::FullWidthSelection, true);
        seleccion.cursor = QTextCursor(bloquePorNumero(it.key()));
        seleccion.cursor.clearSelection();
        selecciones.append(seleccion);
    }

    setExtraSelections(selecciones);
}

