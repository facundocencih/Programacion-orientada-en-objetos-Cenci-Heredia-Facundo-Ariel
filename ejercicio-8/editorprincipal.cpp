#include "editorprincipal.h"

#include "codeeditor.h"
#include "logger.h"
#include "validadorsintaxis.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFocusEvent>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QImage>
#include <QImageWriter>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QTabWidget>
#include <QTransform>
#include <QVBoxLayout>

EditorPrincipal::EditorPrincipal(const Config &config, Logger *logger, QWidget *parent)
    : QWidget(parent), m_config(config), m_logger(logger)
{
    inicializarUI();
    cargarDatos();
    conectarEventos();
}

QString EditorPrincipal::nombrePantalla() const
{
    return "EditorPrincipal";
}

void EditorPrincipal::inicializarUI()
{
    setWindowTitle("Editor multilenguaje");
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("QWidget { background: #edf2f7; color: #1c2633; font-family: Segoe UI; }"
                  "QComboBox, QPushButton { padding: 8px 12px; border: 1px solid #b7c3d0; border-radius: 6px; background: white; }"
                  "QPushButton { background: #165f7f; color: white; border: 0; }"
                  "QTabWidget::pane { border: 1px solid #c4ced9; background: #ffffff; border-radius: 6px; }"
                  "QTabBar::tab { padding: 8px 18px; background: #dbe5ef; border-top-left-radius: 6px; border-top-right-radius: 6px; margin-right: 2px; }"
                  "QTabBar::tab:selected { background: #ffffff; font-weight: 700; }"
                  "QLabel#panelTitulo { font-size: 20px; font-weight: 700; }"
                  "QLabel#diagnostico { padding: 9px; border-radius: 6px; background: #e6f4ea; }");

    auto *raiz = new QHBoxLayout(this);
    raiz->setContentsMargins(18, 18, 18, 18);
    raiz->setSpacing(16);

    auto *zonaEditor = new QVBoxLayout;
    auto *barra = new QHBoxLayout;

    m_lenguajes = new QComboBox(this);
    m_lenguajes->addItems({"C++", "Python", "Java"});
    m_corregir = new QPushButton("Corregir sintaxis", this);
    m_galeria = new QPushButton("Galeria", this);
    m_exportar = new QPushButton("Exportar JPG", this);
    m_diagnostico = new QLabel(this);
    m_diagnostico->setObjectName("diagnostico");
    m_diagnostico->setWordWrap(true);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(crearPestanaEditor("C++"), "C++");
    m_tabs->addTab(crearPestanaEditor("Python"), "Python");
    m_tabs->addTab(crearPestanaEditor("Java"), "Java");

    barra->addWidget(new QLabel("Lenguaje", this));
    barra->addWidget(m_lenguajes);
    barra->addStretch();
    barra->addWidget(m_corregir);
    barra->addWidget(m_galeria);
    barra->addWidget(m_exportar);

    zonaEditor->addLayout(barra);
    zonaEditor->addWidget(m_tabs, 1);
    zonaEditor->addWidget(m_diagnostico);

    raiz->addLayout(zonaEditor, 1);
    raiz->addWidget(crearPanelCurriculum());
}

void EditorPrincipal::conectarEventos()
{
    connect(m_lenguajes, &QComboBox::currentTextChanged, this, &EditorPrincipal::cambiarLenguaje);
    connect(m_tabs, &QTabWidget::currentChanged, this, &EditorPrincipal::cambiarPestana);
    connect(m_exportar, &QPushButton::clicked, this, &EditorPrincipal::exportarJpg);
    connect(m_galeria, &QPushButton::clicked, this, &EditorPrincipal::abrirGaleria);
    connect(m_corregir, &QPushButton::clicked, this, &EditorPrincipal::corregirSintaxis);
    connect(this, &EditorPrincipal::eventoRegistrado, m_logger, &Logger::registrar);
}

void EditorPrincipal::cargarDatos()
{
    const int indice = m_lenguajes->findText(m_config.lenguajeDefault);
    m_lenguajes->setCurrentIndex(indice >= 0 ? indice : 0);
    m_tabs->setCurrentIndex(m_lenguajes->currentIndex());
    m_editores["C++"]->setPlainText("// Bienvenido al editor C++\n");
    m_editores["Python"]->setPlainText("# Bienvenido al editor Python\n");
    m_editores["Java"]->setPlainText("// Bienvenido al editor Java\n");
    m_diagnostico->setText("Listo para editar.");
}

bool EditorPrincipal::validarEstado() const
{
    return editorActual() && !editorActual()->toPlainText().trimmed().isEmpty();
}

void EditorPrincipal::registrarEvento(const QString &descripcion)
{
    emit eventoRegistrado(nombrePantalla() + ": " + descripcion);
}

void EditorPrincipal::cambiarLenguaje(const QString &lenguaje)
{
    for (int i = 0; i < m_tabs->count(); ++i) {
        if (m_tabs->tabText(i) == lenguaje && m_tabs->currentIndex() != i) {
            m_tabs->setCurrentIndex(i);
            break;
        }
    }
    registrarEvento("Lenguaje seleccionado: " + lenguaje);
}

void EditorPrincipal::cambiarPestana(int indice)
{
    const QString lenguaje = m_tabs->tabText(indice);
    if (m_lenguajes->currentText() != lenguaje) {
        m_lenguajes->setCurrentText(lenguaje);
    }
    m_diagnostico->setText("Editando pestana " + lenguaje + ".");
    registrarEvento("Pestana activa: " + lenguaje);
}

void EditorPrincipal::mostrarDiagnostico(int numeroLinea, bool valida, const QString &diagnostico)
{
    m_diagnostico->setText(QString("Linea %1: %2").arg(numeroLinea).arg(diagnostico));
    m_diagnostico->setStyleSheet(valida
                                 ? "background: #e6f4ea; color: #174d2c; padding: 9px; border-radius: 6px;"
                                 : "background: #ffe5e5; color: #8a1f1f; padding: 9px; border-radius: 6px;");
}

void EditorPrincipal::exportarJpg()
{
    if (!validarEstado()) {
        QMessageBox::information(this, "Exportacion", "No hay codigo para exportar.");
        return;
    }

    const QString ruta = rutaExportacionActual();
    if (guardarComoJpg(ruta, editorActual(), lenguajeActual())) {
        registrarEvento("Codigo exportado a " + QFileInfo(ruta).absoluteFilePath());
        QMessageBox::information(
            this,
            "Exportacion",
            "El codigo se exporto correctamente.\n\nCarpeta:\n" + QFileInfo(ruta).absolutePath()
                + "\n\nArchivo:\n" + QFileInfo(ruta).fileName());
    } else {
        registrarEvento("Fallo la exportacion JPG");
        QMessageBox::warning(this, "Exportacion", "No se pudo exportar el codigo.");
    }
}

void EditorPrincipal::abrirGaleria()
{
    auto *dialogo = new QDialog(this);
    dialogo->setWindowTitle("Galeria de exportaciones");
    dialogo->resize(720, 480);

    auto *layout = new QVBoxLayout(dialogo);
    auto *info = new QLabel("Carpeta de exportacion: " + carpetaExportaciones(), dialogo);
    info->setWordWrap(true);
    auto *tabs = new QTabWidget(dialogo);

    const QStringList lenguajes = {"C++", "Python", "Java"};
    const QDir carpeta(carpetaExportaciones());
    const QFileInfoList archivos = carpeta.entryInfoList({"*.jpg", "*.jpeg"}, QDir::Files, QDir::Time);

    for (const QString &lenguaje : lenguajes) {
        auto *lista = new QListWidget(tabs);
        QString filtro = lenguaje.toLower();
        filtro.replace("+", "p");
        for (const QFileInfo &archivo : archivos) {
            if (archivo.fileName().toLower().contains("_" + filtro + "_")) {
                lista->addItem(archivo.fileName() + "    " + archivo.lastModified().toString("dd/MM/yyyy hh:mm"));
            }
        }
        if (lista->count() == 0) {
            lista->addItem("Sin exportaciones todavia.");
        }
        tabs->addTab(lista, lenguaje);
    }

    layout->addWidget(info);
    layout->addWidget(tabs, 1);
    dialogo->exec();
    registrarEvento("Galeria de exportaciones abierta");
}

void EditorPrincipal::corregirSintaxis()
{
    if (!validarEstado()) {
        QMessageBox::information(this, "Correccion", "No hay codigo para corregir.");
        return;
    }

    int correcciones = 0;
    const QString corregido = corregirCodigo(editorActual()->toPlainText(), lenguajeActual(), &correcciones);
    editorActual()->setPlainText(corregido);
    editorActual()->validarLineaActual();

    m_diagnostico->setText(QString("Correccion de sintaxis aplicada: %1 cambio(s).").arg(correcciones));
    m_diagnostico->setStyleSheet("background: #e6f4ea; color: #174d2c; padding: 9px; border-radius: 6px;");
    registrarEvento(QString("Correccion de sintaxis aplicada en %1: %2 cambio(s)").arg(lenguajeActual()).arg(correcciones));
}

CodeEditor *EditorPrincipal::editorActual() const
{
    return m_editores.value(lenguajeActual(), nullptr);
}

QString EditorPrincipal::lenguajeActual() const
{
    return m_tabs ? m_tabs->tabText(m_tabs->currentIndex()) : m_lenguajes->currentText();
}

QString EditorPrincipal::carpetaExportaciones() const
{
    const QFileInfo info(m_config.rutaExportacion);
    const QString carpeta = info.dir().path().isEmpty() ? "exports" : info.dir().path();
    QDir().mkpath(carpeta);
    return QFileInfo(carpeta).absoluteFilePath();
}

QString EditorPrincipal::rutaExportacionActual() const
{
    QString lenguajeArchivo = lenguajeActual().toLower();
    lenguajeArchivo.replace("+", "p");
    const QString nombre = QString("codigo_%1_%2.jpg")
                               .arg(lenguajeArchivo)
                               .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"));
    return QDir(carpetaExportaciones()).absoluteFilePath(nombre);
}

QWidget *EditorPrincipal::crearPestanaEditor(const QString &lenguaje)
{
    auto *contenedor = new QWidget(this);
    auto *layout = new QVBoxLayout(contenedor);
    layout->setContentsMargins(10, 10, 10, 10);

    auto *editor = new CodeEditor(contenedor);
    editor->setPlaceholderText("Escribi codigo. La linea se valida al abandonarla.");
    aplicarValidador(editor, lenguaje);
    m_editores.insert(lenguaje, editor);

    connect(editor, &CodeEditor::lineaValidada, this, &EditorPrincipal::mostrarDiagnostico);
    connect(editor, &CodeEditor::accionEditor, this, &EditorPrincipal::registrarEvento);

    layout->addWidget(editor);
    return contenedor;
}

void EditorPrincipal::aplicarValidador(CodeEditor *editor, const QString &lenguaje)
{
    if (!editor) {
        return;
    }
    if (lenguaje == "Python") {
        editor->setValidador(std::make_unique<ValidadorPython>());
    } else if (lenguaje == "Java") {
        editor->setValidador(std::make_unique<ValidadorJava>());
    } else {
        editor->setValidador(std::make_unique<ValidadorCpp>());
    }
}

bool EditorPrincipal::guardarComoJpg(const QString &ruta, CodeEditor *editor, const QString &lenguaje) const
{
    if (!editor) {
        return false;
    }

    QDir().mkpath(QFileInfo(ruta).absolutePath());
    const QString codigo = editor->toPlainText();
    const QStringList lineas = codigo.split('\n');
    const QFont fuente("Consolas", 16);
    const QFontMetrics metricas(fuente);
    const int margen = 36;
    int anchoTexto = 0;
    for (const QString &linea : lineas) {
        anchoTexto = qMax(anchoTexto, metricas.horizontalAdvance(linea));
    }
    const int ancho = qMax(1100, anchoTexto + 240);
    const int alto = qMax(700, margen * 2 + metricas.height() * (lineas.size() + 2));

    QImage imagen(ancho, alto, QImage::Format_RGB32);
    imagen.fill(QColor("#ffffff"));

    QPainter painter(&imagen);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setFont(fuente);
    painter.setPen(QColor("#102033"));
    painter.drawText(margen, margen, "Exportacion de codigo - " + lenguaje);

    int y = margen + metricas.height() * 2;
    for (int i = 0; i < lineas.size(); ++i) {
        painter.setPen(QColor("#6b7785"));
        painter.drawText(margen, y, QString::number(i + 1).rightJustified(3, ' '));
        painter.setPen(QColor("#102033"));
        painter.drawText(margen + 58, y, lineas.at(i));
        y += metricas.height();
    }
    painter.end();

    QImageWriter writer(ruta, "JPG");
    writer.setQuality(95);
    writer.setText("codigo", codigo);
    writer.setText("lenguaje", lenguaje);
    writer.setText("origen", "Editor multilenguaje ejercicio 8");
    return writer.write(imagen);
}

QString EditorPrincipal::corregirCodigo(const QString &codigo, const QString &lenguaje, int *correcciones) const
{
    int cambios = 0;
    QStringList lineas = codigo.split('\n');
    const QStringList bloquesPython = {"if ", "for ", "while ", "def ", "class ", "elif ", "else", "try", "except", "finally", "with "};
    const QStringList bloquesLlaves = {"if", "for", "while", "switch", "class", "struct", "else", "try", "catch"};

    for (QString &linea : lineas) {
        const QString original = linea;
        QString limpia = linea.trimmed();

        if (limpia.isEmpty() || limpia.startsWith("//") || limpia.startsWith("#")) {
            continue;
        }

        while (limpia.count("(") > limpia.count(")")) {
            linea += ")";
            limpia += ")";
        }
        while (limpia.count("[") > limpia.count("]")) {
            linea += "]";
            limpia += "]";
        }

        if (lenguaje == "Python") {
            for (const QString &bloque : bloquesPython) {
                if (limpia.startsWith(bloque) && !limpia.endsWith(":")) {
                    linea += ":";
                    limpia += ":";
                    break;
                }
            }
            if (linea != original) {
                ++cambios;
            }
            continue;
        }

        bool esBloque = limpia.endsWith("{") || limpia.endsWith("}") || limpia.endsWith(":");
        for (const QString &bloque : bloquesLlaves) {
            if (limpia.startsWith(bloque + " ") || limpia.startsWith(bloque + "(")) {
                esBloque = true;
                break;
            }
        }

        const bool requierePuntoYComa = !esBloque
                                        && (limpia.contains("=") || limpia.contains("cout")
                                            || limpia.contains("return") || limpia.contains("System.out")
                                            || limpia.contains("new "));
        if (requierePuntoYComa && !limpia.endsWith(";")) {
            linea += ";";
        }

        if (linea != original) {
            ++cambios;
        }
    }

    if (correcciones) {
        *correcciones = cambios;
    }
    return lineas.join('\n');
}

QWidget *EditorPrincipal::crearPanelCurriculum()
{
    auto *panel = new QWidget(this);
    panel->setFixedWidth(330);
    panel->setStyleSheet("QWidget { background: #ffffff; border: 1px solid #cbd5df; border-radius: 8px; }"
                         "QLabel { border: 0; background: transparent; }");
    auto *layout = new QVBoxLayout(panel);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(10);

    auto *foto = new QLabel(panel);
    foto->setFixedSize(132, 132);
    foto->setAlignment(Qt::AlignCenter);
    QPixmap pix(":/assets/profile.jpg");
    QTransform giro;
    giro.rotate(90);
    pix = pix.transformed(giro, Qt::SmoothTransformation);

    QPixmap circular(132, 132);
    circular.fill(Qt::transparent);
    QPainter painter(&circular);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath path;
    path.addEllipse(0, 0, 132, 132);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, pix.scaled(132, 132, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    foto->setPixmap(circular);

    auto *nombre = new QLabel("Facundo Cenci Heredia Ariel", panel);
    nombre->setObjectName("panelTitulo");
    nombre->setWordWrap(true);
    auto *rol = new QLabel("Computer Engineering student | C++ and Qt desktop development", panel);
    rol->setWordWrap(true);
    auto *descripcion = new QLabel("Estudiante de Ingenieria Informatica en 3er ano en Universidad Blas Pascal, basado en Cordoba, Argentina. Busco mi primera experiencia profesional en desarrollo de software.", panel);
    descripcion->setWordWrap(true);
    auto *habilidades = new QLabel("Habilidades\nC++ intermedio\nGit basico\nDocker en aprendizaje\nQt Creator y Visual Studio\nLogica de programacion y estructuras de datos", panel);
    habilidades->setWordWrap(true);
    auto *contacto = new QLabel("Contacto\nfacundocenci@gmail.com\n+54 351 517 7835\nCordoba, Argentina", panel);
    contacto->setWordWrap(true);

    layout->addWidget(foto, 0, Qt::AlignHCenter);
    layout->addWidget(nombre);
    layout->addWidget(rol);
    layout->addSpacing(8);
    layout->addWidget(descripcion);
    layout->addWidget(habilidades);
    layout->addWidget(contacto);
    layout->addStretch();
    return panel;
}

void EditorPrincipal::keyPressEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Save)) {
        exportarJpg();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_F11) {
        isFullScreen() ? showNormal() : showFullScreen();
        registrarEvento("F11 alterno pantalla completa");
        return;
    }
    QWidget::keyPressEvent(event);
}

void EditorPrincipal::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click en ventana principal x=%1 y=%2").arg(event->position().x()).arg(event->position().y()));
    QWidget::mousePressEvent(event);
}

void EditorPrincipal::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Editor adaptado a %1x%2").arg(width()).arg(height()));
    QWidget::resizeEvent(event);
}

void EditorPrincipal::closeEvent(QCloseEvent *event)
{
    const QMessageBox::StandardButton respuesta = QMessageBox::question(
        this, "Salir", "Deseas exportar el codigo a JPG antes de salir?",
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    if (respuesta == QMessageBox::Cancel) {
        event->ignore();
        return;
    }
    if (respuesta == QMessageBox::Yes) {
        guardarComoJpg(rutaExportacionActual(), editorActual(), lenguajeActual());
    }
    registrarEvento("Cierre confirmado desde editor");
    event->accept();
}

void EditorPrincipal::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Editor principal recibio foco");
    QWidget::focusInEvent(event);
}

void EditorPrincipal::focusOutEvent(QFocusEvent *event)
{
    if (editorActual()) {
        editorActual()->validarLineaActual();
    }
    registrarEvento("Editor principal perdio foco y valido la linea actual");
    QWidget::focusOutEvent(event);
}
