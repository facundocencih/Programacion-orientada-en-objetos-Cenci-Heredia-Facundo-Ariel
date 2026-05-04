#include "validadorsintaxis.h"

#include <QStringList>

static bool terminaComoBloque(const QString &linea)
{
    return linea.endsWith("{") || linea.endsWith("}") || linea.endsWith(":");
}

static bool requierePuntoYComa(const QString &linea)
{
    if (linea.isEmpty() || linea.startsWith("//") || linea.startsWith("#")) {
        return false;
    }
    if (terminaComoBloque(linea)) {
        return false;
    }
    const QStringList prefijos = {"if", "for", "while", "switch", "class", "struct", "else", "try", "catch"};
    for (const QString &prefijo : prefijos) {
        if (linea.startsWith(prefijo + " ") || linea.startsWith(prefijo + "(")) {
            return false;
        }
    }
    return linea.contains("=") || linea.contains("cout") || linea.contains("return")
           || linea.contains("System.out") || linea.contains("new ");
}

QString ValidadorCpp::lenguaje() const
{
    return "C++";
}

ResultadoValidacion ValidadorCpp::validarLinea(const QString &linea, int) const
{
    const QString limpia = linea.trimmed();
    if (limpia.count("(") != limpia.count(")") || limpia.count("[") != limpia.count("]")) {
        return {false, "Revisa parentesis o corchetes: parece faltar un cierre."};
    }
    if (requierePuntoYComa(limpia) && !limpia.endsWith(";")) {
        return {false, "En C++ esta linea probablemente necesita punto y coma."};
    }
    return {true, "Linea C++ valida."};
}

QString ValidadorPython::lenguaje() const
{
    return "Python";
}

ResultadoValidacion ValidadorPython::validarLinea(const QString &linea, int) const
{
    const QString limpia = linea.trimmed();
    if (limpia.isEmpty() || limpia.startsWith("#")) {
        return {true, "Linea sin instrucciones para validar."};
    }
    if (limpia.endsWith(";")) {
        return {false, "En Python normalmente no se usa punto y coma al final."};
    }
    const QStringList bloques = {"if ", "for ", "while ", "def ", "class ", "elif ", "else", "try", "except", "finally", "with "};
    for (const QString &bloque : bloques) {
        if (limpia.startsWith(bloque) && !limpia.endsWith(":")) {
            return {false, "Las estructuras de Python deben terminar con dos puntos."};
        }
    }
    if (limpia.count("(") != limpia.count(")") || limpia.count("[") != limpia.count("]")) {
        return {false, "Revisa parentesis o corchetes: parece faltar un cierre."};
    }
    return {true, "Linea Python valida."};
}

QString ValidadorJava::lenguaje() const
{
    return "Java";
}

ResultadoValidacion ValidadorJava::validarLinea(const QString &linea, int) const
{
    const QString limpia = linea.trimmed();
    if (limpia.count("(") != limpia.count(")") || limpia.count("[") != limpia.count("]")) {
        return {false, "Revisa parentesis o corchetes: parece faltar un cierre."};
    }
    if (requierePuntoYComa(limpia) && !limpia.endsWith(";")) {
        return {false, "En Java esta linea probablemente necesita punto y coma."};
    }
    return {true, "Linea Java valida."};
}
