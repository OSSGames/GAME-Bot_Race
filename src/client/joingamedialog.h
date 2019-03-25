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

#ifndef JOINGAMEDIALOG_H
#define JOINGAMEDIALOG_H

#include <QDialog>

namespace Ui {
    class JoinGameDialog;
}

namespace BotRace {
namespace Client {

struct JoinDetails_T {
    QString name;
    int robot;

    QString ip;
    int port;
};

class JoinGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinGameDialog(QWidget *parent = 0);
    ~JoinGameDialog();

    JoinDetails_T getDetails();

private:
    Ui::JoinGameDialog *ui;
};

}
}

#endif // JOINGAMEDIALOG_H
