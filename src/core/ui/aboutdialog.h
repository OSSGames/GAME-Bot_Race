/*
 * Copyright 2011 Jörg Ehrichs <joerg.ehichs@gmx.de>
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

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class AboutDialog;
}

namespace BotRace {
namespace Core {

/**
 * @brief Simple about dialog to give some basic details about the game
 *
*/
class AboutDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief constructor
    */
    explicit AboutDialog( );

    /**
     * @brief destructor
     *
    */
    ~AboutDialog();

private:
    Ui::AboutDialog *ui; /**< Pointer to the ui form */
};

}
}

#endif // ABOUTDIALOG_H
