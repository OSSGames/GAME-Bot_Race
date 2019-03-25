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

#ifndef PLISTWIDGETITEM_H
#define PLISTWIDGETITEM_H

#include <QObject>
#include <QListWidgetItem>

namespace BotRace {
namespace Core {
class AbstractClient;
}
namespace Network
{
class PListWidgetItem : public QObject, public QListWidgetItem
{
    Q_OBJECT
public:
    explicit PListWidgetItem(Core::AbstractClient *client);

    bool isEqualTo(Core::AbstractClient *c);
private slots:
    void updateName();

private:
    Core::AbstractClient *m_client;

};

}
}

#endif // PLISTWIDGETITEM_H
