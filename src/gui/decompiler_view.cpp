#include "main_window.h"
#include <QtWidgets/QMenu>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtCore/QRegularExpression>
#include <QtGui/QTextCharFormat>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QClipboard>

namespace debugger {

class CppSyntaxHighlighter : public QSyntaxHighlighter {
public:
    explicit CppSyntaxHighlighter(QTextDocument* parent = nullptr) : QSyntaxHighlighter(parent) {
        setup_highlighting_rules();
    }

private:
    void highlightBlock(const QString& text) override {
        foreach (const HighlightingRule& rule, highlighting_rules) {
            QRegularExpressionMatchIterator match_iterator = rule.pattern.globalMatch(text);
            while (match_iterator.hasNext()) {
                QRegularExpressionMatch match = match_iterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

    void setup_highlighting_rules() {
        HighlightingRule rule;

        // Keywords
        keyword_format.setForeground(QColor(86, 156, 214));
        keyword_format.setFontWeight(QFont::Bold);
        QStringList keywords;
        keywords << "\\bint\\b" << "\\bchar\\b" << "\\bshort\\b" << "\\blong\\b"
                << "\\bunsigned\\b" << "\\bsigned\\b" << "\\bfloat\\b" << "\\bdouble\\b"
                << "\\bvoid\\b" << "\\bif\\b" << "\\belse\\b" << "\\bwhile\\b"
                << "\\bfor\\b" << "\\breturn\\b" << "\\bstruct\\b" << "\\bunion\\b"
                << "\\benum\\b" << "\\btypedef\\b" << "\\bstatic\\b" << "\\bconst\\b";

        foreach (const QString& pattern, keywords) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keyword_format;
            highlighting_rules.append(rule);
        }

        // Function calls
        function_format.setForeground(QColor(220, 220, 170));
        function_format.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\()");
        rule.format = function_format;
        highlighting_rules.append(rule);

        // Numbers
        number_format.setForeground(QColor(181, 206, 168));
        rule.pattern = QRegularExpression("\\b0[xX][0-9A-Fa-f]+\\b|\\b\\d+\\b");
        rule.format = number_format;
        highlighting_rules.append(rule);

        // Strings
        string_format.setForeground(QColor(214, 157, 133));
        rule.pattern = QRegularExpression("\".*\"");
        rule.format = string_format;
        highlighting_rules.append(rule);

        // Comments
        comment_format.setForeground(QColor(106, 153, 85));
        comment_format.setFontItalic(true);
        rule.pattern = QRegularExpression("//[^\n]*");
        rule.format = comment_format;
        highlighting_rules.append(rule);

        // Multi-line comments
        rule.pattern = QRegularExpression("/\\*.*\\*/");
        rule.format = comment_format;
        highlighting_rules.append(rule);

        // Variable names
        variable_format.setForeground(QColor(156, 220, 254));
        rule.pattern = QRegularExpression("\\b[a-z_][a-z0-9_]*\\b");
        rule.format = variable_format;
        highlighting_rules.append(rule);
    }

    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlighting_rules;

    QTextCharFormat keyword_format;
    QTextCharFormat function_format;
    QTextCharFormat comment_format;
    QTextCharFormat string_format;
    QTextCharFormat number_format;
    QTextCharFormat variable_format;
};

DecompilerView::DecompilerView(QWidget* parent) : QTextEdit(parent) {
    // Set monospace font
    QFont font("Consolas", 10);
    font.setStyleHint(QFont::Monospace);
    setFont(font);
    
    setReadOnly(true);
    setLineWrapMode(QTextEdit::NoWrap);
    
    // Setup syntax highlighting
    setup_syntax_highlighting();
    
    // Set placeholder text
    setPlaceholderText("Decompiled code will appear here...\n\nLoad a binary and analyze functions to see decompiled C code.");
}

void DecompilerView::set_decompiled_code(const std::string& code) {
    QString q_code = QString::fromStdString(code);
    setPlainText(q_code);
    
    // Apply syntax highlighting
    if (syntax_highlighter) {
        syntax_highlighter->rehighlight();
    }
}

void DecompilerView::clear_code() {
    clear();
    setPlaceholderText("Decompiled code will appear here...\n\nLoad a binary and analyze functions to see decompiled C code.");
}

void DecompilerView::setup_syntax_highlighting() {
    syntax_highlighter = new CppSyntaxHighlighter(document());
}

void DecompilerView::append_decompiled_function(const std::string& function_code) {
    if (!toPlainText().isEmpty()) {
        append("\n\n"); // Add separation between functions
    }
    append(QString::fromStdString(function_code));
}

void DecompilerView::contextMenuEvent(QContextMenuEvent* event) {
    QMenu* menu = createStandardContextMenu();
    
    // Add custom actions
    menu->addSeparator();
    
    QAction* copy_all = menu->addAction("Copy All Code");
    connect(copy_all, &QAction::triggered, [this] {
        QApplication::clipboard()->setText(toPlainText());
    });
    
    QAction* export_code = menu->addAction("Export to File...");
    export_code->setEnabled(false); // TODO: Implement file export
    
    menu->addSeparator();
    
    QAction* analyze_function = menu->addAction("Analyze Current Function");
    connect(analyze_function, &QAction::triggered, [this] {
        emit function_analysis_requested();
    });
    
    QAction* goto_assembly = menu->addAction("Go to Assembly");
    connect(goto_assembly, &QAction::triggered, [this] {
        emit goto_assembly_requested();
    });
    
    menu->exec(event->globalPos());
    delete menu;
}

void DecompilerView::navigate_to_function(const std::string& function_name) {
    QString search_text = QString::fromStdString(function_name);
    
    // Search for the function name in the document
    QTextDocument* doc = document();
    QTextCursor cursor = doc->find(search_text);
    
    if (!cursor.isNull()) {
        setTextCursor(cursor);
        ensureCursorVisible();
        
        // Highlight the function name
        cursor.select(QTextCursor::WordUnderCursor);
        setTextCursor(cursor);
    }
}

void DecompilerView::highlight_line(int line_number) {
    QTextCursor cursor(document());
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line_number - 1);
    cursor.select(QTextCursor::LineUnderCursor);
    
    setTextCursor(cursor);
    ensureCursorVisible();
    
    // Create highlighting
    QList<QTextEdit::ExtraSelection> selections;
    QTextEdit::ExtraSelection selection;
    
    selection.format.setBackground(QColor(255, 255, 0, 100)); // Light yellow
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = cursor;
    
    selections.append(selection);
    setExtraSelections(selections);
}

void DecompilerView::add_analysis_comment(const std::string& comment) {
    QString current_text = toPlainText();
    if (!current_text.isEmpty()) {
        current_text += "\n";
    }
    
    current_text += QString("/* Analysis: %1 */").arg(QString::fromStdString(comment));
    setPlainText(current_text);
    
    // Move cursor to end
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End);
    setTextCursor(cursor);
    ensureCursorVisible();
}

QString DecompilerView::get_selected_text() {
    return textCursor().selectedText();
}

void DecompilerView::find_text(const QString& text) {
    QTextDocument* doc = document();
    QTextCursor cursor = doc->find(text, textCursor());
    
    if (!cursor.isNull()) {
        setTextCursor(cursor);
        ensureCursorVisible();
    } else {
        // Search from beginning if not found from current position
        cursor = doc->find(text);
        if (!cursor.isNull()) {
            setTextCursor(cursor);
            ensureCursorVisible();
        }
    }
}

} // namespace debugger 