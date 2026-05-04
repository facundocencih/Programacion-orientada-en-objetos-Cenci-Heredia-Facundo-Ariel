#ifndef EDITORPRINCIPAL_H
#define EDITORPRINCIPAL_H

#include "config.h"
#include "pantalla.h"

#include <QMap>
#include <QWidget>

class CodeEditor;
class QLabel;
class QComboBox;
class QPushButton;
class Logger;
class QTabWidget;

class EditorPrincipal : public QWidget, public Pantalla
{
    Q_OBJECT

public:
    EditorPrincipal(const Config &config, Logger *logger, QWidget *parent = nullptr);

    QString nombrePantalla() const override;
    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() const override;
    void registrarEvento(const QString &descripcion) override;

signals:
    void eventoRegistrado(const QString &descripcion);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void cambiarLenguaje(const QString &lenguaje);
    void cambiarPestana(int indice);
    void mostrarDiagnostico(int numeroLinea, bool valida, const QString &diagnostico);
    void exportarJpg();
    void abrirGaleria();
    void corregirSintaxis();

private:
    CodeEditor *editorActual() const;
    QString lenguajeActual() const;
    QString carpetaExportaciones() const;
    QString rutaExportacionActual() const;
    QWidget *crearPestanaEditor(const QString &lenguaje);
    void aplicarValidador(CodeEditor *editor, const QString &lenguaje);
    bool guardarComoJpg(const QString &ruta, CodeEditor *editor, const QString &lenguaje) const;
    QString corregirCodigo(const QString &codigo, const QString &lenguaje, int *correcciones) const;
    QWidget *crearPanelCurriculum();

    Config m_config;
    Logger *m_logger;
    QComboBox *m_lenguajes;
    QTabWidget *m_tabs;
    QMap<QString, CodeEditor *> m_editores;
    QLabel *m_diagnostico;
    QPushButton *m_exportar;
    QPushButton *m_galeria;
    QPushButton *m_corregir;
};

#endif // EDITORPRINCIPAL_H
