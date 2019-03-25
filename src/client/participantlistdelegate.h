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

#ifndef PARTICIPANTLISTDELEGATE_H
#define PARTICIPANTLISTDELEGATE_H

#include <QStyledItemDelegate>

class QPainter;

namespace BotRace {

namespace Renderer {
    class GameTheme;
}

namespace Client {

/**
 * @brief A delegate to show an svg for the participant item in the list
 */
class ParticipantListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ParticipantListDelegate(QObject *parent = 0);

    /**
     * @brief Sets the renderer where the svg item is queried from
     * @param renderer teh used renderer
     */
    void setRenderer(Renderer::GameTheme *renderer);

    /**
     * @brief Paints the delegate with all important values as retrieved from the model
     * @param painter the painter
     * @param option some options
     * @param index the participant which wil be drawn
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option,const QModelIndex &index) const;

    /**
     * @brief Returns thesize of the item. The size is determined by the current gui scale and the original size of the backgrounditem
     * @param option some options (unused)
     * @param index some index (unused)
     * @return the corect item size
     */
    QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index ) const;

private:
    Renderer::GameTheme *m_renderer;
    
};

}
}

#endif // PARTICIPANTLISTDELEGATE_H
