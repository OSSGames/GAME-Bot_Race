/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QDialog>

class QVBoxLayout;
class QWebView;
class QDialogButtonBox;

namespace BotRace {
namespace Core {

class HelpBrowser : public QDialog
{
    Q_OBJECT
    
public:
    enum HelpFile {
        HELP_GAME,
        HELP_EDITOR,
        HELP_SERVER
    };

    explicit HelpBrowser(HelpFile file, QWidget *parent = 0);
    
private:
    void setupDialog();
    QVBoxLayout *verticalLayout;
    QWebView *webView;
    QDialogButtonBox *buttonBox;
};

}
}

#endif // HELPBROWSER_H
