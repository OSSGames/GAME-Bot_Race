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

#ifndef PROGRAMMINGWIDGET_H
#define PROGRAMMINGWIDGET_H

#include <QWidget>

#include "engine/cards.h"

class QLabel;
class QPushButton;

namespace BotRace {
namespace Core {
class AbstractClient;
}
namespace Renderer {
class GameTheme;
}
namespace Client {

class ProgrammingWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProgrammingWidget( QWidget *parent = 0 );

    void setRenderer(Renderer::GameTheme *renderer);
    void resetWidget();
    void setPlayer( Core::AbstractClient *player );

public slots:
    void addCardAt( ushort slot, BotRace::Core::GameCard_T card );
    void removeCard( ushort slot );
    void lockSlots( ushort minSlot );
    void changeSize();
    void updateFlagGoal( ushort nextGoal );
    void updateLifeToken( ushort newLife );
    void updateDamageToken( ushort newDamage );
    void enablePowerDown( bool powerDown );

    /**
     * @brief updated the programcard items with the current active GameCard_T items from the player deck.
     *
     * Will be called when the FLOOR_RANDOMIZER is activated and replac one of the cards.
     */
    void replaceProgramCards();

protected:
    void paintEvent( QPaintEvent *event );
    void dragEnterEvent( QDragEnterEvent *event );
    void dragMoveEvent( QDragMoveEvent *event );
    void dropEvent( QDropEvent *event );
    void mousePressEvent( QMouseEvent *event );
    void mouseDoubleClickEvent( QMouseEvent *event );

signals:
    void sendCardTo( ushort slot, BotRace::Core::GameCard_T card );
    void sizeChaged(int newHight);

private:
    void setupWidget();

    Core::AbstractClient *m_player;
    Renderer::GameTheme *m_renderer;

    QLabel *m_flagLabel;
    QLabel *m_life1Label;
    QLabel *m_life2Label;
    QLabel *m_life3Label;
    QLabel *m_life4Label;
    QLabel *m_life5Label;
    QLabel *m_life6Label;
    QLabel *m_damage1Label;
    QLabel *m_damage2Label;
    QLabel *m_damage3Label;
    QLabel *m_damage4Label;
    QPushButton *m_programmingButton;
    QPushButton *m_powerdownButton;

    QSizeF m_cardSize;
};

}
}

#endif // PROGRAMMINGWIDGET_H
